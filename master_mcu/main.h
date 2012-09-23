/*
 * main.h
 *
 *  Created on: Sep 20, 2012
 *      Author: jcmcclu2
 */

#ifndef MAIN_H_
#define MAIN_H_

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

// USART
#define read_byte_uart() UCA0RXBUF
#define send_byte_uart(x) UCA0TXBUF = (x)
void send_uart(char* str);
void init_uart(void);
void uart_rx(void);
//void uart_tx(void);

// I2C
#define i2c_start() UCB0CTL1 |= UCTXSTT
#define i2c_start_tx() UCB0CTL1 |= UCTR + UCTXSTT

#define i2c_stop() UCB0CTL1 |= UCTXSTP
#define i2c_stop_tx() i2c_stop(); IFG2 &= ~UCB0TXIFG

#define i2c_tx_byte(x) UCB0TXBUF = (x)
#define i2c_set_slave(x) UCB0I2CSA = (x)

#define I2C_SERVER1 1
#define I2C_SERVER2 2
#define I2C_SERVER3 3
#define I2C_SERVER4 4
void i2c_tx();
void i2c_rx();
//void i2c_start();
//void i2c_stop();

#endif /* MAIN_H_ */
