#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rtc.h"
#include "twi.h"
#include "gpio.h"

#define INT1_P(dir) GPIO(D, 3, dir)

struct rtc rtc;
volatile uint8_t rtc_int;

static uint8_t rtc_write_buff(uint8_t addr, void *data, uint8_t size);
static uint8_t rtc_write_alarm1();
static uint8_t rtc_write_ctrlstat();
static uint8_t rtc_write_status();


void rtc_init()
{
    /* INT1 - input, pullup */
    INT1_P(DDR)  = 0;
    INT1_P(PORT) = 1;

    /* INT1 - falling edge */
    MCUCR = (MCUCR & (~_BV(ISC10))) | _BV(ISC11);
    GICR |= _BV(INT1);

    /* Read in all registers */
    rtc_read_all();

    /* Configure alarm 1 to fire off every second */
    rtc.alarm1.m1 = 1;
    rtc.alarm1.m2 = 1;
    rtc.alarm1.m3 = 1;
    rtc.alarm1.m4 = 1;

    rtc_write_alarm1();

    /* Enable alarm1 interrupt, clear int flag  */
    rtc.control.a1ie = 1;
    rtc.status.a1f = 0;
    rtc_write_ctrlstat();
}

void rtc_reset_int()
{
    rtc.status.a1f = 0;
    rtc_write_status();
    rtc_int = 0;
}

uint8_t rtc_read_all()
{
    uint8_t *ptr;
    uint8_t i;

    if (twi_start(TW_START)) {
        return 1;
    }

    if (twi_addr(RTC_I2C_ADDR, TW_WRITE)) {
        return 2;
    }


    if (twi_transmit_data(0)) {
        return 3;
    }


    if (twi_start(TW_REP_START)) {
        return 4;
    }


    if (twi_addr(RTC_I2C_ADDR, TW_READ)) {
        return 5;
    }

    ptr = (uint8_t *)&rtc;
    for (i = 0; i < sizeof(rtc); i++) {
        *ptr = twi_receive_data();
        ptr ++;
    }

    twi_nack();

    twi_stop();
    return 0;
}

static uint8_t rtc_write_buff(uint8_t addr, void *data, uint8_t size)
{
    uint8_t *ptr = data;
    uint8_t i;

    if (twi_start(TW_START)) {
        return 1;
    }

    if (twi_addr(RTC_I2C_ADDR, TW_WRITE)) {
        return 2;
    }

    if (twi_transmit_data(addr)) {
        return 3;
    }

    for (i = 0; i < size; i++) {
        twi_transmit_data(*ptr);
        ptr ++;
    }

    twi_stop();
    return 0;
}


uint8_t rtc_write_all()
{
    return rtc_write_buff(0, &rtc, sizeof(rtc));
}

static uint8_t rtc_write_alarm1()
{
    return rtc_write_buff( (int)&rtc.alarm1 - (int)&rtc,
                           &rtc.alarm1, sizeof(rtc.alarm1));
}

static uint8_t rtc_write_ctrlstat()
{
    return rtc_write_buff( (int)&rtc.control - (int)&rtc,
                           &rtc.control, sizeof(rtc.control) + sizeof(rtc.status));
}

static uint8_t rtc_write_status()
{
    return rtc_write_buff( (int)&rtc.status - (int)&rtc,
                           &rtc.status, sizeof(rtc.status));
}

ISR(INT1_vect)
{
    rtc_int = 1;
}

