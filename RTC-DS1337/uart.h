#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>

extern volatile char uart_buffer[];
extern volatile uint8_t uart_rxf;

void uart_init();
void uart_reset_rx();
int uart_putchar(char c, FILE *stream);


#endif
