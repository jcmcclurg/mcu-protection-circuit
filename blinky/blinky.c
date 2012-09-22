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
#define MAX_VOLTAGE (1024*5/16)

void main(void)
{
	CSL_init();                     // Activate Grace-generated configuration

//	stop_watchdog();
	/*
     * The MCU crashed and has summarily been reset.
     * This is considered catastrophic failure since
     * the whole point of this MCU is to protect more
     * expensive and complicated circuitry.
     *
     * If the MCU ever crashes, it will attempt to
     * disconnect the server it was protecting, report
     * the crash to the main supervisory MCU, then
     * do nothing until a device reset.
     */
    // Check if WDT+ caused the prior reset condition
//    if (IFG1 & WDTIFG) {
//    	stop_watchdog();
//        // WDT+ initiated the prior reset condition
//    	on_MCU_crash(); // Handle crash
//        IFG1 &= ~WDTIFG; // Software clear WDTIFG
//    }
//    else
//    {
    	//green_led_on();

		start_adc();
		start_pwm();
		set_pwm_duty_cycle(0);

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
			}
		}
//    }
}

/*
 * Interrupt routines for use with the Grace configuration
 */

// Watchdog Timer: gets called when the MCU
void on_MCU_crash(void)
{
	// Attempt to switch off everything again. Light up "catastrophic failure" LED.
}

// I2C
void i2c_tx(void)
{
}

void i2c_rx(void)
{
	char b = i2c_rx_byte();
//	toggle_green_led();
}

void i2c_start_condition(void)
{
	UCB0STAT &= ~(UCSTTIFG);
}
void i2c_stop_condition(void)
{
	UCB0STAT &= ~(UCSTPIFG);
}

// ADC: gets called when the ADC has a value ready to be read
void adc_ready(void)
{
	if(read_adc() > MAX_VOLTAGE && STATE == STABLE) {
		STATE = OFFLOADING;
	}
}

void overvoltage_condition(void)
{
	switch_out_server();
	slow_short_input_terminals();
	turn_on_scr();
}

void slow_short_input_terminals(void)
{
	start_pwm();
	stop_pwm();
}
