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
#include "itoa.h"

// Uart stuff
char uart_last_byte = 0;
char uart_flags;


// Own stuff
#define COMMAND_STATUS (1)
#define COMMAND_VOLTS  (1<<1)
char command_flags = 0;
char current_server = 0;
char alarm_flags    = 0;
char alarm_count = 0;
char str_buffer[32];
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
    green_led_on();
    while(1);
}

void alarm_received(void)
{
	if(ALARMPORT & SERVER1_PIN && !(alarm_flags & SERVER1_ALARM))
	{
		P1IFG &= ~SERVER1_PIN;
		alarm_flags |= SERVER1_ALARM;
		update_voltage();
	}

	if(ALARMPORT & SERVER2_PIN && !(alarm_flags & SERVER2_ALARM))
	{
		P1IFG &= ~SERVER2_PIN;
		alarm_flags |= SERVER2_ALARM;
		update_voltage();
	}

	if(ALARMPORT & SERVER3_PIN && !(alarm_flags & SERVER3_ALARM))
	{
		P1IFG &= ~SERVER3_PIN;
		alarm_flags |= SERVER3_ALARM;
		update_voltage();
	}

	if(ALARMPORT & SERVER4_PIN && !(alarm_flags & SERVER4_ALARM))
	{
		P1IFG &= ~SERVER4_PIN;
		alarm_flags |= SERVER4_ALARM;
		update_voltage();
	}
}

void update_voltage(void)
{
	if(alarm_count < 2)
	{
		green_led_off();
		red_led_on();
		alarm_count++;
		int voltage = 48 - (12*alarm_count);

		itoa(voltage,str_buffer,10);
		send_uart(str_buffer);
		send_uart("\r\n");
	}
	else
	{
		green_led_on();
		red_led_on();
		send_uart("STACK FAIL\r\n");
		power_supply_off();
	}
}
