#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

/* Real-time clock data structures. Part: DS1337 */


#define RTC_I2C_ADDR   0b1101000


struct rtc_time {
    uint8_t second:4;
    uint8_t second10:3;
    uint8_t reserved1:1;
    uint8_t minute:4;
    uint8_t minute10:3;
    uint8_t reserved2:1;
    uint8_t hour:4;
    uint8_t hour10:2;
    uint8_t is12h:1;
    uint8_t reserved3:1;
    uint8_t dow:3;
    uint8_t reserved4: 5;
    uint8_t day:4;
    uint8_t day10:2;
    uint8_t reserved5:2;
    uint8_t month:4;
    uint8_t month10:1;
    uint8_t reserved6:2;
    uint8_t century:1;
    uint8_t year:4;
    uint8_t year10:4;
} __attribute__((__packed__));

struct rtc_alarm1 {
    uint8_t second:4;
    uint8_t second10:3;
    uint8_t m1:1;
    uint8_t minute:4;
    uint8_t minute10:3;
    uint8_t m2:1;
    uint8_t hour:4;
    uint8_t hour10:2;
    uint8_t is12h:1;
    uint8_t m3:1;
    uint8_t day:4;
    uint8_t day10:2;
    uint8_t dy:1;
    uint8_t m4:1;
} __attribute__((__packed__));


struct rtc_alarm2 {
    uint8_t minute:4;
    uint8_t minute10:3;
    uint8_t m2:1;
    uint8_t hour:4;
    uint8_t hour10:2;
    uint8_t is12h:1;
    uint8_t m3:1;
    uint8_t day:4;
    uint8_t day10:2;
    uint8_t dy:1;
    uint8_t m4:1;
} __attribute__((__packed__));


struct rtc_control {
    uint8_t a1ie:1;
    uint8_t a2ie:1;
    uint8_t intcn:1;
    uint8_t rs1:1;
    uint8_t rs2:1;
    uint8_t reserved:2;
    uint8_t eosc:1;
} __attribute__((__packed__));


struct rtc_status {
    uint8_t a1f:1;
    uint8_t a2f:1;
    uint8_t reserved:5;
    uint8_t osf:1;
} __attribute__((__packed__));

struct rtc {
    struct rtc_time time;
    struct rtc_alarm1 alarm1;
    struct rtc_alarm2 alarm2;
    struct rtc_control control;
    struct rtc_status status;
} __attribute__((__packed__));

extern struct rtc rtc;
extern volatile uint8_t rtc_int;

void rtc_init();
void rtc_reset_int();
uint8_t rtc_read_all();
uint8_t rtc_write_all();


#endif
