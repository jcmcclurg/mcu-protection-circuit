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

#define NUM_VOLTAGES 10
#define RESPONSE_BUFFER_SIZE 2

/*
 * Defines to ease the programming process.
 */

// Red LED on Pin 1.0
#define led_off()    (P1OUT &= ~BIT0)
#define led_on()     (P1OUT |=  BIT0)
#define toggle_led() (P1OUT ^=  BIT0)

#define MAX_VOLTAGE 291 // This is set to correspond to 15 volts.
                        // The microcontroller sees this as 15/16 volts.
                        // Because the Vcc of the microcontroller is 3.3 volts
                        // due to the step-down resistor divider. This gets
                        // stored in the register as 1024*(15/16)/3.3

//#define PROPORTION 8192
#define INITIAL_VOLTAGE_SETPOINT 246 // 1024*(12.7/16)/3.3
//#define VOLT_ERROR_THRESH_START 5
//#define VERROR_THRESH 2
//#define VOLT_ERROR_THRESH_INC 5
//#define VOLTAGE_LEEWAY 23

// Notice that the PWM and ADC use the same timer

// PWM
//#define start_pwm() (TA0CCTL1 = CM_0 + CCIS_0 + OUTMOD_7) // Tie the timer PWM output to a pin
//#define pwm_output_off() (TA0CCTL1 = 0)
#define PWM_START_DUTY 700 // This duty ratio determined experimentally
#define PWM_MAX_DUTY   1000
#define TIMER_TICKS_PER_VOLTAGE_INCREMENT 41 // (10 seconds) * (1 kilohertz) / INITIAL_VOLTAGE_SETPOINT
#define read_pwm_duty_cycle() (TA0CCR1)
#define set_pwm_duty_cycle(r) (TA0CCR1 = (r))
#define stop_pwm() (TA0CTL &= ~(MC1 + MC0)) // Clear MCx bits to stop timer

// ADC
//#define start_adc() (TA0CTL |= MC_1) // Start timer in up mode
#define stop_adc() (ADC10CTL0 &= ~ENC) // Disable ADC
#define read_adc() (ADC10MEM) // Read ADC conversion result from ADC10MEM

// IO
#define switch_in_server() (P2OUT |= BIT3)
#define switch_out_server() (P2OUT &= ~BIT3)
#define turn_on_scr() (P2OUT |= BIT4)
#define scr_pin_low() (P2OUT &= ~BIT4)

#define alarm_high() (P2OUT |= BIT2)
#define alarm_low() (P2OUT &= ~BIT2)

/*
 * Interrupt routines for use with the Grace configuration
 */

// ADC
void adc_ready();

#endif /* LED_H_ */
