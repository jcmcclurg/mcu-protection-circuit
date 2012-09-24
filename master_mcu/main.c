/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/csl/CSL.h>


#include "main.h"
#include "uart.h"

// I2C stuff
#define I2C_RX   (1<<2)
#define I2C_TX   (1<<3)
#define I2C_TXRX (1<<4)
char flags = 0;
char* i2cSendPointer = "";
unsigned char i2cReceiveCount = 0;
char* i2cReceivePointer = "";
unsigned char i2cResponseLength = 0;


// Uart stuff
char uart_byte_before_last = 0;
char uart_last_byte = 0;
char uart_flags;


// Own stuff
#define COMMAND_STATUS (1)
#define COMMAND_VOLTS  (1<<1)
char command_flags = 0;
char current_server = 0;

const unsigned char serverAddresses[NUM_SERVERS] = {I2C_SERVER1, I2C_SERVER2, I2C_SERVER3, I2C_SERVER4};
server_state server_states[NUM_SERVERS];
int server_voltages[NUM_SERVERS];
char server_response_buffer[SERVER_RESPONSE_BUFFER_SIZE];


/*
 *  ======== main ========
 */
void main(int argc, char *argv[])
{
    CSL_init();                     // Activate Grace-generated configuration
//    init_uart();

    top_led_off();
    bottom_led_off();
    red_led_off();

    // Tell the uart library which flags buffer and last byte buffer to populate
    init_uart(&uart_last_byte, &uart_flags);
    send_uart("\r\nMaster MCU\r\n");

    while(1)
    {
    	if(uart_flags & UART_RX)
    	{
    		uart_flags &= ~UART_RX;
    		send_uart("UART_RX{");
    		send_byte_uart(uart_last_byte);

    		if(uart_last_byte >= '1' && uart_last_byte <= '4')
    		{
    			current_server = uart_last_byte - '1';
				if(uart_byte_before_last == 's')
				{
					command_flags |= COMMAND_STATUS;
					i2c_communicate("s", serverAddresses[current_server], server_response_buffer, sizeof(server_state));
				}
				else if(uart_byte_before_last == 'v')
				{
					command_flags |= COMMAND_VOLTS;
					i2c_communicate("v", serverAddresses[current_server], server_response_buffer, sizeof(int));
				}
    		}
    		send_uart("}\r\n");

    		uart_byte_before_last = uart_last_byte;
    	}

    	if(flags & I2C_TX)
		{
			flags &= ~I2C_TX;
			send_uart("I2C_TX\r\n");
		}

    	if(flags & I2C_RX)
    	{
    		flags &= ~I2C_RX;
    		send_uart("I2C_RX\r\n");
    	}

    	if(flags & I2C_TXRX)
    	{
    		flags &= ~I2C_TXRX;
    		send_uart("I2C_TXRX{");
    		int i;
    		for(i = 0; i < SERVER_RESPONSE_BUFFER_SIZE; i++)
    		{
    			send_byte_uart(server_response_buffer[i]);
    		}
			send_uart("}\r\n");

			if(command_flags & COMMAND_STATUS)
			{
				command_flags &= ~COMMAND_STATUS;
				memcpy(&(server_states[current_server]),server_response_buffer,sizeof(server_state));

				send_uart("Server ");
				send_byte_uart(current_server + '1');
				if(server_states[current_server] == STABLE)
				{
					send_uart(" STABLE\r\n");
				}
				else if(server_states[current_server] == OFFLOADING)
				{
					send_uart(" OFFLOADING\r\n");
				}
				else// if(server_states[current_server] == OFF)
				{
					send_uart(" OFF\r\n");
				}
			}

			if(command_flags & COMMAND_VOLTS)
			{
				command_flags &= ~COMMAND_VOLTS;
				memcpy(&(server_voltages[NUM_SERVERS]),server_response_buffer,sizeof(int));

				send_uart("Server ");
				send_byte_uart(current_server + '1');
				send_uart(" voltage read.\r\n");
			}
    	}
    }
}

// I2C
void i2c_send_data(char* str, unsigned char addr)
{
	i2c_communicate(str,addr,0,0);
}

// This function works as follows:
// 1. Wait for previous messages (if any) to finish by checking whether i2cSendPointer points to null character.
// 2. Load the i2cSendPointer with string
// 3. Use tx_ready interrupt to shift out the bytes of string until you reach a null character. At this point, stop.
void i2c_communicate(char* str, unsigned char addr, char* response, unsigned char responseLength)
{
	if(*str != 0)
	{
		// Allow previous message to finish.
		while(!(*i2cSendPointer == 0));

		i2cReceivePointer = response;
		i2cResponseLength = responseLength;
		i2cReceiveCount = 0;

		i2cSendPointer = str;
		i2c_set_slave(addr);
		i2c_start_tx();
	}
}

void i2c_tx()
{
	IFG2 &= ~UCB0TXIFG; // Not really necessary since I think that sending a byte to the TX buffer already clears the interrupt.
	flags |= I2C_TX;

	top_led_on();

	if(*i2cSendPointer != 0)
	{
		i2c_tx_byte(*i2cSendPointer);
		i2cSendPointer++;
	}
	else
	{
		// IFG2 &= ~UCB0TXIFG; // Make sure you un-comment this if you comment out the top interrupt clearing statement
		if(i2cResponseLength == 0)
		{
			i2c_stop();
		}
		else
		{
			i2c_start_rx();
		}
	}
	top_led_off();
}

void i2c_rx()
{
	IFG2 &= ~UCB0RXIFG;  // Not really necessary because I'm pretty sure reading the RX buffer clears the interrupt automatically.
	flags |= I2C_RX;
	bottom_led_on();

	// Issue the stop command on the last byte
	if(i2cReceiveCount < i2cResponseLength-1)
	{
		*i2cReceivePointer = i2c_rx_byte();
		i2cReceivePointer++;
		if(i2cReceiveCount == i2cResponseLength-2)
		{
			i2c_stop();
		}

		// Clear interrupt flag.
		//IFG2 &= ~UCB0RXIFG;  // Make sure you un-comment this if you comment out the top interrupt clearing statement
	}
	else if(i2cReceiveCount == i2cResponseLength-1)
	{
		flags |= I2C_TXRX;
		*i2cReceivePointer = i2c_rx_byte();
	}
	i2cReceiveCount++;
	bottom_led_off();
}
