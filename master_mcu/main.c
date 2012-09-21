/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/csl/CSL.h>

#include "main.h"
/*
 *  ======== main ========
 */
void main(int argc, char *argv[])
{
    CSL_init();                     // Activate Grace-generated configuration
    green_led_off();
    red_led_off();
    green_led_on();
    red_led_on();
    while(1);
}

// USART
void uart_rx(void)
{
	green_led_off();
	red_led_off();
	safe_send_uart(UCA0RXBUF);
}

void uart_tx(void)
{
	green_led_on();
	red_led_off();
}

// I2C
void i2c_tx()
{

}

void i2c_rx()
{

}

void i2c_start()
{

}

void i2c_stop()
{

}
