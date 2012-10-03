/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/csl/CSL.h>


#include "master_mcu.h"
#include "uart.h"

// Uart stuff
char uart_last_byte = 0;
char uart_flags;


// Own stuff
#define COMMAND_STATUS (1)
#define COMMAND_VOLTS  (1<<1)
char command_flags = 0;
char current_server = 0;
char alarm_flags    = 0;
server_state server_states[NUM_SERVERS];

/*
 *  ======== main ========
 */
void main(int argc, char *argv[])
{
    CSL_init();                     // Activate Grace-generated configuration

    int i;
    for(i=0;i<NUM_SERVERS;i++)
    {
    	server_states[i] = STABLE;
    }
    green_led_off();
    red_led_off();

    // Tell the uart library which flags buffer and last byte buffer to populate
    init_uart(&uart_last_byte, &uart_flags);
    send_uart("\r\nMaster MCU\r\n");

    while(1)
    {
    	if(uart_flags & UART_RX)
    	{
    		uart_flags &= ~UART_RX;
    		send_byte_uart(uart_last_byte);
    	}
    }
}

void alarm_received(void)
{


	red_led_on();
	if(ALARMPORT & SERVER1_PIN && !(alarm_flags & SERVER1_ALARM))
	{
		P1IFG &= ~SERVER1_PIN;

		alarm_flags |= SERVER1_ALARM;
		send_uart("Server 1 alarm!\r\n");
	}

	if(ALARMPORT & SERVER2_PIN && !(alarm_flags & SERVER2_ALARM))
	{
		P1IFG &= ~SERVER2_PIN;

		alarm_flags |= SERVER2_ALARM;
		send_uart("Server 2 alarm!\r\n");
	}

	if(ALARMPORT & SERVER3_PIN && !(alarm_flags & SERVER3_ALARM))
	{
		P1IFG &= ~SERVER3_PIN;

		alarm_flags |= SERVER3_ALARM;
		send_uart("Server 3 alarm!\r\n");
	}

	if(ALARMPORT & SERVER4_PIN && !(alarm_flags & SERVER4_ALARM))
	{
		P1IFG &= ~SERVER4_PIN;

		alarm_flags |= SERVER4_ALARM;
		send_uart("Server 4 alarm!\r\n");
	}
}
