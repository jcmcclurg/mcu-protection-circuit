/*
 * main.h
 *
 *  Created on: Sep 20, 2012
 *      Author: jcmcclu2
 */

#ifndef MAIN_H_
#define MAIN_H_

typedef enum server_state_enum {
	STABLE,
	OFFLOADING,
	OFF
} server_state;

#define NUM_SERVERS 4

#define red_led_off()    (P1OUT &= ~BIT0)
#define red_led_on()     (P1OUT |=  BIT0)
#define toggle_red_led() (P1OUT ^=  BIT0)

#define top_led_on()     (P2OUT &= ~BIT0)
#define top_led_off()    (P2OUT |=  BIT0)
#define toggle_top_led() (P2OUT ^=  BIT0)

#define bottom_led_on()     (P2OUT &= ~BIT1)
#define bottom_led_off()    (P2OUT |=  BIT1)
#define toggle_bottom_led() (P2OUT ^=  BIT1)

#endif /* MAIN_H_ */
