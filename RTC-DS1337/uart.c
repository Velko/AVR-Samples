#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#include "defines.h"

#include <util/setbaud.h>

//1234567890123456
//t1307221203816rn
// yyMMddWhhmmss
volatile char uart_buffer[16];
volatile uint8_t uart_idx;
volatile uint8_t uart_rxf;

FILE uart_str;

#ifdef __AVR_ATmega328P__

/* The difference between "classic" ATmega8 and ATmega328p is that
   the registers and bits are named differently */

#define UBRRH   UBRR0H
#define UBRRL   UBRR0L
#define UCSRA   UCSR0A
#define U2X     U2X0
#define UCSRC   UCSR0C
#define UCSZ1   UCSZ01
#define UCSZ0   UCSZ00
#define UCSRB   UCSR0B
#define RXEN    RXEN0
#define TXEN    TXEN0
#define RXCIE   RXCIE0
#define UDRE    UDRE0
#define UDR     UDR0
#define RXC     RXC0
#define USART_RXC_vect  USART_RX_vect

#endif

void uart_init()
{
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

#if USE_2X
    UCSRA |= _BV(U2X);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSRC = _BV(UCSZ1) | _BV(UCSZ0); /* 8-bit data */
    UCSRB = _BV(RXEN) | _BV(TXEN);   /* Enable RX and TX */

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

