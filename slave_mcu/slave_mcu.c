
//*****************************************************************************
//  Blink LED Example
//
//  Description: No longer a blinking LED. ;-)
//*****************************************************************************

/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/csl/CSL.h>

/*
 *  ======== main ========
 */
#include "blinky.h"
#include "uart.h"
#define MAX_VOLTAGE (1024*5/16)

char uart_flags = 0;
char uart_last_byte = 0;
int pastVoltages[NUM_VOLTAGES];
int* pastVoltagePointer = pastVoltages;
int voltageSum = -1;

// TODO: Use Timer1 to define an exact ramp-rate for the PWM in ms.
void main(void)
{
	CSL_init();                     // Activate Grace-generated configuration
    	red_led_off();
        top_led_off();
        bottom_led_off();

        // Tell the uart library which flags buffer and last byte buffer to populate
	init_uart(&uart_last_byte, &uart_flags);
	send_uart("\r\nSlave node 1\r\n");

        switch_in_server();
	start_adc();

	while (1) {
		if (STATE == OFFLOADING) {
			set_pwm_duty_cycle(read_pwm_duty_cycle() + 1);

			// Faster than modulus (and avoids slowness warning)
			if (read_pwm_duty_cycle() == 100) {
				STATE = OFF;
				stop_pwm();

				turn_on_scr();
			} else {
				int i;
				for (i = 0; i < 500; i++) {}
			}
			top_led_on();
			bottom_led_on();
		}
	}
}

// ADC: gets called when the ADC has a value ready to be read
void adc_ready(void)
{
	toggle_top_led();

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
		bottom_led_on();
		switch_out_server();
		stop_adc();

		// Begin slow-shorting the terminals
		set_pwm_duty_cycle(0);
		start_pwm();

		STATE = OFFLOADING;
	}
}
