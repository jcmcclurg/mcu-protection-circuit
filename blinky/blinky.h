/*
 * led.h
 *
 *  Created on: Sep 15, 2012
 *      Author: jcmcclu2
 */

#ifndef LED_H_
#define LED_H_

typedef enum server_state_enum {
	STABLE,
	OFFLOADING,
	OFF
} server_state;

server_state STATE = STABLE;

/*
 * Defines to ease the programming process.
 */
#define red_led_off()    (P1OUT &= ~BIT0)
#define red_led_on()     (P1OUT |=  BIT0)
#define toggle_red_led() (P1OUT ^=  BIT0)

#define top_led_on()     (P2OUT &= ~BIT0)
#define top_led_off()    (P2OUT |=  BIT0)
#define toggle_top_led() (P2OUT ^=  BIT0)

#define bottom_led_on()     (P2OUT &= ~BIT1)
#define bottom_led_off()    (P2OUT |=  BIT1)
#define toggle_bottom_led() (P2OUT ^=  BIT1)

// Notice that the PWM and ADC use the same timer

// PWM
#define start_pwm() (TA0CCTL1 = CM_0 + CCIS_0 + OUTMOD_7) // Tie the timer PWM output to a pin
//#define pwm_output_off() (TA0CCTL1 = 0)
#define read_pwm_duty_cycle() (TA0CCR1)
#define set_pwm_duty_cycle(r) (TA0CCR1 = (r))
#define stop_pwm() (TA0CTL &= ~(MC1 + MC0)) // Clear MCx bits to stop timer

// WATCHDOG
//#define start_watchdog() (WDTCTL = WDTPW)         // Clear hold (and all other) bits
//#define stop_watchdog() (WDTCTL = WDTPW + WDTHOLD) // Set hold bit and clear others
//#define feed_watchdog() (WDTCTL = WDTCTL + WDTCNTCL)  // Set WDT+ counter clear bit (instruction clears other bits)

// ADC
#define start_adc() (TA0CTL |= MC_1) // Start timer in up mode
#define stop_adc() (ADC10CTL0 &= ~ENC) // Disable ADC
#define read_adc() (ADC10MEM) // Read ADC conversion result from ADC10MEM

// IO
#define switch_in_server() (P2OUT |= (BIT5))
#define switch_out_server() (P2OUT &= ~(BIT5))
#define turn_on_scr() (P2OUT |= (BIT4))

/*
 * Interrupt routines for use with the Grace configuration
 */

// Watchdog Timer
//void on_MCU_crash();

// I2C
#define i2c_rx_byte() UCB0RXBUF
void i2c_tx();
void i2c_rx();
void i2c_start_condition();
void i2c_stop_condition();

// ADC
void adc_ready();

/*
 * Our functions
 */

// High-level
void overvoltage_condition(void);
void slow_short_input_terminals(void);

#endif /* LED_H_ */
