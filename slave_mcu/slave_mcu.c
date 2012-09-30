
//*****************************************************************************
//  Blink LED Example
//
//  Description: No longer a blinking LED. ;-)
//*****************************************************************************

/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>
#include <stdio.h>
/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/csl/CSL.h>

/*
 *  ======== main ========
 */
#include "slave_mcu.h"
#include "uart.h"
#include "itoa.h"

char uart_flags = 0;
char uart_last_byte = 0;
char pwm_ticker = 0;
int pastVoltages[NUM_VOLTAGES];
int* pastVoltagePointer = pastVoltages;
int voltageSum = -1;
char str_buffer[32];

void main(void)
{
	CSL_init();                     // Activate Grace-generated configuration

	// Initial state
	switch_in_server();
	led_on();

	// Tell the uart library which flags buffer and last byte buffer to populate
	init_uart(&uart_last_byte, &uart_flags);
	send_uart("\r\nSlave node 4 UART test.\r\nDO NOT CONNECT TO PC WHILE MAIN VOLTAGE IS ON!!!\r\n");

	while (1)
	{
		if(uart_flags & UART_RX)
		{
			uart_flags &= ~UART_RX;

			if(uart_last_byte == 'p')
			{
				stop_pwm();
				send_uart("\r\nPWM Stopped\r\n");
			}
			else if(uart_last_byte == '+')
			{
				set_pwm_duty_cycle(read_pwm_duty_cycle() + 1);
				send_uart("\r\nPWM Increased\r\n");
				itoa(read_pwm_duty_cycle(),str_buffer,10);
				send_uart(str_buffer);
				send_uart("\r\n");
			}
			else if(uart_last_byte == '-')
			{
				set_pwm_duty_cycle(read_pwm_duty_cycle() - 1);
				send_uart("\r\nPWM Decreased\r\n");
				itoa(read_pwm_duty_cycle(),str_buffer,10);
				send_uart(str_buffer);
				send_uart("\r\n");
			}
			else
			{
				send_byte_uart(uart_last_byte);
			}
		}
	}
}

void timer0_tick(void)
{
   if(STATE == OFFLOADING)
   {
	   if(pwm_ticker >= TIMER_TICKS_PER_PWM_INCREMENT)
	   {
		   toggle_led();
		   set_pwm_duty_cycle(read_pwm_duty_cycle() + 1);

		   if (read_pwm_duty_cycle() >= PWM_END_DUTY)
		   {
			   set_pwm_duty_cycle(PWM_MAX_DUTY);
			   STATE = OFF;
			   led_off();
			   stop_pwm();
			   turn_on_scr();
		   }
		   pwm_ticker = 0;
	   }
	   pwm_ticker++;
   }
}

// ADC: gets called when the ADC has a value ready to be read
void adc_ready(void)
{
	// Calculate initial sum, keeping it negative to indicate that it is not complete
	if(voltageSum <= -1)
	{
		*pastVoltagePointer = read_adc();
		if(pastVoltagePointer >= pastVoltages+NUM_VOLTAGES-1)
		{
			// Remove the initial -1.
			voltageSum++;

			// Flip the sign.
			voltageSum *= -1;
		}
		else
		{
			voltageSum -= *pastVoltagePointer;
			pastVoltagePointer++;
		}
	}

	// Adjust initial sum to implement moving average. This way, computation time of the moving
	// average is not dependent upon the size of average.
	else
	{
		pastVoltagePointer++;
		if(pastVoltagePointer >= pastVoltages+NUM_VOLTAGES)
		{
			pastVoltagePointer = pastVoltages;
		}
		// Remove the overwritten voltage from the sum
		voltageSum -= *pastVoltagePointer;

		// Add the new voltage to the sum
		*pastVoltagePointer = read_adc();
		voltageSum += *pastVoltagePointer;
	}

	if(voltageSum > MAX_VOLTAGE*NUM_VOLTAGES && STATE == STABLE) {
		// Begin slow-shorting the terminals
		STATE = OFFLOADING;

		switch_out_server();
		alarm_high();
		stop_adc();
		set_pwm_duty_cycle(PWM_START_DUTY);
	}
}
