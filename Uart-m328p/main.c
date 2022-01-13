#include <avr/io.h>
#include <avr/pgmspace.h>

#include "uart.h"

char buffer[64];

int uart_getchar(FILE *stream);

int main()
{
    uart_init();
    stdout = stdin = serial;

    printf_P(PSTR("Hello, World!\r\n"));

    for (;;) {

        fgets(buffer, 64, serial);

        printf_P(PSTR("Echo: %s"), buffer);
    }
}
