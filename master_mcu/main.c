/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/csl/CSL.h>


#include "main.h"

const unsigned char serverAddresses[NUM_SERVERS] = {I2C_SERVER1, I2C_SERVER2, I2C_SERVER3, I2C_SERVER4};

/* The send_uart(char* stringToSend) configuration works as follows:
 * 1. Initialize sendPointer to point to beginning of null-terminated string
 * 2. Send first byte
 * 3. When TX is ready again, increment pointer and send next byte if it is not null.
 *
 * Be careful when using this inside interrupt service routines. You should not expect more than the first
 * byte to get sent until the ISR you are currently in exits.
 *
 * Also, it's a generally a bad idea to make multiple sequential calls to send_uart, since the call is
 * non-blocking and will overwrite whatever is already in the buffer.
 */
char* uartSendPointer = "";
#define send_uart(x) uartSendPointer = (x); send_byte_uart(*uartSendPointer)

// The send_i2c(char* stringToSend, char address) configuration works the same way as send_uart
char* i2cSendPointer = "";
#define send_i2c(x, a) i2cSendPointer = (x); i2c_set_slave(a); i2c_start_tx()

//char receiveBuffer[9];
//char* receivePointer = sendBuffer;
#define read_uart(x) receiveBuffer


/*
 *  ======== main ========
 */
void main(int argc, char *argv[])
{
    CSL_init();                     // Activate Grace-generated configuration
    init_uart();

    green_led_off();
    red_led_off();

    green_led_on();
    red_led_on();
    while(1);
}

// USART
void init_uart(void)
{
	//receiveBuffer[0] = 0;
}

// TODO: Implement circular buffer for storing command before it is parsed
void uart_rx(void)
{
	//*receivePointer = read_byte_uart();
	char c = read_byte_uart();
	if(c == 'j')
	{
		send_i2c("HI THERE", I2C_SERVER1);
	}
	else
	{
	   send_uart("No serial commands have been implemented.\r\n");
	}
}

void uart_tx(void)
{
	if(*uartSendPointer != 0)
	{
		uartSendPointer++;
		send_byte_uart(*uartSendPointer);
	}
}

// I2C
void i2c_tx()
{
	if(*i2cSendPointer != 0)
	{
		i2cSendPointer++;
		send_byte_uart(*i2cSendPointer);
		i2c_tx_byte(*i2cSendPointer);
	}
	else
	{
		i2c_stop_tx();
	}
}

void i2c_rx()
{

}
