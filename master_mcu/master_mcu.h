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
#define ALARMPORT P2IN
#define SERVER1_PIN BIT0
#define SERVER2_PIN BIT1
#define SERVER3_PIN BIT2
#define SERVER4_PIN BIT3

#define SERVER1_ALARM 1
#define SERVER2_ALARM (1<<1)
#define SERVER3_ALARM (1<<2)
#define SERVER4_ALARM (1<<3)

#define red_led_off()    (P1OUT &= ~BIT0)
#define red_led_on()     (P1OUT |=  BIT0)
#define toggle_red_led() (P1OUT ^=  BIT0)

#define green_led_off()    (P1OUT &= ~BIT6)
#define green_led_on()      (P1OUT |= BIT6)
#define toggle_green_led() (P1OUT ^=  BIT6)

#define power_supply_off() (P2OUT |= BIT4)

void update_voltage(void);

#endif /* MAIN_H_ */
