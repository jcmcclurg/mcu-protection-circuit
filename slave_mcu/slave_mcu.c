
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
char voltage_ticker = 0;

int voltageSum = 0;
int pastVoltages[NUM_VOLTAGES];
int* pastVoltagePointer = pastVoltages;

//int voltage_error_thresh = VOLT_ERROR_THRESH_START;
int voltage_setpoint = INITIAL_VOLTAGE_SETPOINT;
char str_buffer[32];

void main(void)
{
	CSL_init();                     // Activate Grace-generated configuration

	// Initial state
	switch_in_server();
	led_on();

	int i;
	for(i=0;i<NUM_VOLTAGES;i++)
	{
		pastVoltages[i] = 0;
	}

	// Tell the uart library which flags buffer and last byte buffer to populate
	init_uart(&uart_last_byte, &uart_flags);
	send_uart("\r\nSlave node x UART test.\r\nDO NOT CONNECT TO PC WHILE MAIN VOLTAGE IS ON!!!\r\n");

	while (1)
	{
		if(uart_flags & UART_RX)
		{
			uart_flags &= ~UART_RX;

			if(uart_last_byte == 'v')
			{
				send_uart("Voltage = ");
				itoa(voltageSum,str_buffer,10);
				send_uart(str_buffer);
				send_uart(". Set voltage = ");
				itoa(voltage_setpoint*NUM_VOLTAGES,str_buffer,10);
				send_uart(str_buffer);
				send_uart("\r\n");
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
			else if(uart_last_byte == '>')
			{
				voltage_setpoint++;
				itoa(voltage_setpoint,str_buffer,10);
				send_uart(str_buffer);
				send_uart("\r\n");
			}
			else if(uart_last_byte == '<')
			{
				voltage_setpoint--;
				itoa(voltage_setpoint,str_buffer,10);
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
	   if(voltage_ticker >= TIMER_TICKS_PER_VOLTAGE_INCREMENT)
	   {
		   if(voltage_setpoint == 0)
		   {
			   turn_on_scr();
			   STATE = OFF;
			   stop_adc();
			   led_off();
			   // TODO: Stop timer.
		   }
		   else
		   {
			   toggle_led();
			   //voltage_error_thresh = VOLT_ERROR_THRESH_START;
			   voltage_setpoint--;
			   voltage_ticker = 0;
		   }
	   }

/*	   if(voltage_ticker & 0xF == 0)
	   {
		   voltage_error_thresh += VOLT_ERROR_THRESH_INC;
	   }*/
	   voltage_ticker++;
   }
}

// ADC: gets called when the ADC has a value ready to be read
void adc_ready(void)
{
	pastVoltagePointer++;
	if(pastVoltagePointer >= pastVoltages+NUM_VOLTAGES)
	{
		pastVoltagePointer = pastVoltages;
	}
	// Remove the overwritten voltage from the sum
	voltageSum -= *pastVoltagePointer;


	if(STATE == STABLE )
	{
		// Add the new voltage to the sum
		*pastVoltagePointer = (int) read_adc();
		voltageSum += *pastVoltagePointer;

		if(voltageSum > NUM_VOLTAGES*MAX_VOLTAGE)
		{
			switch_out_server();
			alarm_high();

			// Begin slow-shorting the terminals
			voltageSum = 0;
			int i;
			for(i=0;i<NUM_VOLTAGES;i++)
			{
				pastVoltages[i] = 0;
			}

			STATE = OFFLOADING;

			set_pwm_duty_cycle(PWM_START_DUTY);
		}
	}

	else if(STATE == OFFLOADING)
	{
		int delta = *pastVoltagePointer;

		// Add the new voltage to the sum
		*pastVoltagePointer = (((int)read_adc()) - voltage_setpoint);
		voltageSum += *pastVoltagePointer;

		// Get the average derivative
		delta = *pastVoltagePointer - delta;

		if(voltageSum < -NUM_VOLTAGES/2 && read_pwm_duty_cycle() > 0 && delta <= 0)
		{
			//if(((int)read_pwm_duty_cycle()) - (voltageSum/PROPORTION + 1) >= 0)
			//set_pwm_duty_cycle(((int)read_pwm_duty_cycle()) - (voltageSum/PROPORTION + 1));
			if(read_pwm_duty_cycle() > 0)
				set_pwm_duty_cycle(read_pwm_duty_cycle() - 1);
			//else
				//set_pwm_duty_cycle(0);
		}

		else if(voltageSum > NUM_VOLTAGES/2 && read_pwm_duty_cycle() < PWM_MAX_DUTY && delta >= 0)
		{
			if(read_pwm_duty_cycle() < PWM_MAX_DUTY)
				set_pwm_duty_cycle(read_pwm_duty_cycle() + 1);
			//else
				//set_pwm_duty_cycle(PWM_MAX_DUTY);
		}
	}
}
