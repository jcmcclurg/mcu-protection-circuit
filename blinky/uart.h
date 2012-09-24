/*
 * uart.h
 *
 *  Created on: Sep 23, 2012
 *      Author: jcmcclu2
 */

#ifndef UART_H_
#define UART_H_

#define UART_RX  (1)
#define UART_TX  (1<<1)

// USART
//#define UART_BUFFER_LEN 8
#define read_byte_uart()  (UCA0RXBUF)
#define send_byte_uart(x) while(!(IFG2 & UCA0TXIFG)); UCA0TXBUF = (x)
void init_uart(char* cmd, char* flgs);
void send_uart(char* str);
//void init_uart(void);
void uart_rx(void);
//void uart_tx(void);



#endif /* UART_H_ */
