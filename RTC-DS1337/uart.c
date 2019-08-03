#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#include "defines.h"

//1234567890123456
//t1307221203816rn
// yyMMddWhhmmss
volatile char uart_buffer[16];
volatile uint8_t uart_idx;
volatile uint8_t uart_rxf;

FILE uart_str;

void uart_init()
{
#if F_CPU < 2000000UL && defined(U2X)
    UCSRA = _BV(U2X);             /* improve baud rate error by using 2x clk */
    UBRRL = (F_CPU / (8UL * UART_BAUD)) - 1;
#else
    UBRRL = (F_CPU / (16UL * UART_BAUD)) - 1;
#endif
    UCSRB = _BV(TXEN) | _BV(RXEN); /* tx/rx enable */

    /* UART interrupt */
    UCSRB |= _BV(RXCIE);

    uart_reset_rx();

    /* Set up UART as stdout */
    fdev_setup_stream(&uart_str, uart_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &uart_str;
}

void uart_reset_rx()
{
    uart_idx = 0;
    uart_rxf = 0;
}

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int uart_putchar(char c, FILE *stream)
{
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

ISR(USART_RXC_vect)
{
    while (bit_is_set(UCSRA, RXC)) {
        char c = UDR;
        if (c == '\r' || c == '\n') {
            c = 0;
            uart_rxf = 1;
        }
        if (uart_idx < sizeof(uart_buffer))
            uart_buffer[uart_idx++] = c;
        else
            uart_rxf = 1;
    }
}

