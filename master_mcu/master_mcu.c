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

// Uart stuff
char uart_byte_before_last = 0;
char uart_last_byte = 0;
char uart_flags;


// Own stuff
#define COMMAND_STATUS (1)
#define COMMAND_VOLTS  (1<<1)
char command_flags = 0;
char current_server = 0;
server_state server_states[NUM_SERVERS];

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
    }
}
