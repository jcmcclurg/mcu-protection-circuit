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

#define SERVER_RESPONSE_BUFFER_SIZE 2
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

// I2C
#define i2c_start_rx() (UCB0CTL1 = (UCB0CTL1 | UCTXSTT) & (~UCTR))
#define i2c_start_tx() (UCB0CTL1 |= UCTR + UCTXSTT)

#define i2c_stop() (UCB0CTL1 |= UCTXSTP)

#define i2c_tx_byte(x) (UCB0TXBUF = (x))
#define i2c_rx_byte() (UCB0RXBUF)
#define i2c_set_slave(x) (UCB0I2CSA = (x))

#define I2C_SERVER1 1
#define I2C_SERVER2 2
#define I2C_SERVER3 3
#define I2C_SERVER4 4

void i2c_tx();
void i2c_rx();
void i2c_send_data(char* str, unsigned char addr);
void i2c_communicate(char* str, unsigned char addr, char* response, unsigned char responseLength);
//void i2c_start();
//void i2c_stop();

#endif /* MAIN_H_ */
