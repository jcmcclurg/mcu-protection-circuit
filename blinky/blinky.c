
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

#define I2C_RX   (1<<2)
#define I2C_TX   (1<<3)

char i2c_flags = 0;
char uart_flags = 0;
char uart_last_byte = 0;
int pastVoltages[NUM_VOLTAGES];
int* pastVoltagePointer = pastVoltages;
int voltageSum = -1;

char current_command = '?';
char response_buffer[RESPONSE_BUFFER_SIZE];
char txCount = 0;

// TODO: Use Timer1 to define an exact ramp-rate for the PWM in ms.
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
			// Debugging purposes only. Doesn't work too well since the i2c rx and tx happens faster than the while loop.
			/*else if(i2c_flags & I2C_TX)
			{
				i2c_flags &= ~I2C_TX;
				send_uart("I2C_TX{");
				send_byte_uart(current_command);
				send_uart("}\r\n");
			}
			else if(i2c_flags & I2C_RX)
			{
				i2c_flags &= ~I2C_RX;
				send_uart("I2C_RX{");
				send_byte_uart(current_command);
				send_uart("}\r\n");
			}*/
		}
//    }
}

/*
 * Interrupt routines for use with the Grace configuration
 */

// Watchdog Timer: gets called when the MCU
//void on_MCU_crash(void)
//{
//	// Attempt to switch off everything again. Light up "catastrophic failure" LED.
//}

// I2C
void i2c_tx(void)
{
	IFG2 &= ~UCB0TXIFG; // Probably not necessary since I think writing to the TX buffer clears the flag.
	i2c_flags |= I2C_TX;

	if(current_command == 's' || current_command == 'v')
	{
		if(txCount <= RESPONSE_BUFFER_SIZE-1)
		{
			i2c_tx_byte(response_buffer[txCount]);
			txCount++;
		}

		// If the server asks for too much data, send it some zeros.
		else
		{
			red_led_on();
			i2c_tx_byte(0);
		}
	}

	// This shouldn't ever happen. It's when the server gets an invalid i2c command, which asks for data.
	else
	{
		red_led_on();
		i2c_tx_byte('?');
	}
}

void i2c_rx(void)
{
	IFG2 &= ~UCB0RXIFG; // Probably not necessary since I think reading from RX buffer clears the flag.
	i2c_flags |= I2C_RX;
	current_command = i2c_rx_byte();

	if(current_command == 's')
	{
		txCount = 0;
		memcpy(response_buffer,&STATE,sizeof(server_state));
	}
	else if(current_command == 'v')
	{
		int j = read_adc();
		txCount = 0;
		memcpy(response_buffer,&j,sizeof(int));
	}
}

void i2c_start_condition(void)
{
	// Clear start condition flag
	UCB0STAT &= ~(UCSTTIFG);
}
void i2c_stop_condition(void)
{
	// Clear stop condition flag
	UCB0STAT &= ~(UCSTPIFG);
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
