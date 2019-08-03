#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "twi.h"
#include "rtc.h"




void update_time(volatile char *tb)
{
    rtc.time.century = 0;
    rtc.time.year10  = tb[0] - '0';
    rtc.time.year    = tb[1] - '0';
    rtc.time.month10 = tb[2] - '0';
    rtc.time.month   = tb[3] - '0';
    rtc.time.day10   = tb[4] - '0';
    rtc.time.day     = tb[5] - '0';
    rtc.time.dow     = tb[6] - '0';

    rtc.time.is12h = 0;
    rtc.time.hour10   = tb[7] - '0';
    rtc.time.hour     = tb[8] - '0';
    rtc.time.minute10 = tb[9] - '0';
    rtc.time.minute   = tb[10] - '0';
    rtc.time.second10 = tb[11] - '0';
    rtc.time.second   = tb[12] - '0';

    rtc.status.osf = 0;
}

int main()
{
    uart_init();
    twi_init();
    rtc_init();

    sei();

    for (;;) {

        if (rtc_int) {

            rtc_read_all();
            rtc_reset_int();

            printf_P(PSTR("%d 20%d%d-%d%d-%d%d %d "),
                     rtc.status.osf,
                     rtc.time.year10,
                     rtc.time.year,
                     rtc.time.month10,
                     rtc.time.month,
                     rtc.time.day10,
                     rtc.time.day,
                     rtc.time.dow);

            printf_P(PSTR("%d%d:%d%d:%d%d\r\n"),
                     rtc.time.hour10,
                     rtc.time.hour,
                     rtc.time.minute10,
                     rtc.time.minute,
                     rtc.time.second10,
                     rtc.time.second);
        }

        if (uart_rxf) {

            printf_P(PSTR("Echo: %s\r\n"), uart_buffer);

            if (uart_buffer[0] == 't')
                update_time(uart_buffer + 1);

            rtc_write_all();

            uart_reset_rx();
        }


        while (!rtc_int && !uart_rxf);
    }
}



