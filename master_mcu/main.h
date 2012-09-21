/*
 * main.h
 *
 *  Created on: Sep 20, 2012
 *      Author: jcmcclu2
 */

#ifndef MAIN_H_
#define MAIN_H_

#define send_uart(x) UCA0TXBUF = (x)
#define safe_send_uart(x) while (!(IFG2 & UCA0TXIFG)); send_uart((x))

#define green_led_on() (P1OUT |= (BIT0))
#define green_led_off() (P1OUT &= ~(BIT0))

#define red_led_on() (P1OUT |= (BIT6))
#define red_led_off() (P1OUT &= ~(BIT6))

// USART
void uart_rx(void);
void uart_tx(void);

// I2C
void i2c_tx();
void i2c_rx();
void i2c_start();
void i2c_stop();

#endif /* MAIN_H_ */
