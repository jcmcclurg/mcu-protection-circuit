/*
 * uart.c
 *
 *  Created on: Sep 23, 2012
 *      Author: jcmcclu2
 */

// USART
//void init_uart(void)
//{
//	int i;
//	for(i=0; i < UART_BUFFER_LEN; i++)
//	{
//		uartBuffer[i] = 0;
//	}
//}
#include <msp430.h>

#include "uart.h"

char* _current_command;
char* _uart_flags;

void init_uart(char* cmd, char* flgs)
{
	_current_command = cmd;
	_uart_flags = flgs;
}

void send_uart(char* str)
{
	//toggle_red_led();
	while(*str != 0)
	{
		send_byte_uart(*str);
		str++;
	}
}

void uart_rx(void)
{
	IFG2 &= ~UCA0RXIFG;
	*_uart_flags |= UART_RX;
	*_current_command = read_byte_uart();
}
