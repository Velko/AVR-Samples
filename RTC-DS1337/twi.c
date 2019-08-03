#include <stdio.h>
#include <avr/io.h>
#include "defines.h"
#include "gpio.h"
#include "twi.h"

#define SCL_P(dir)  GPIO(C, 5, dir)
#define SDA_P(dir)  GPIO(C, 4, dir)

#include <util/twi.h>

void twi_init()
{
    /* SCL - input, pullup */
    SCL_P(DDR)  = 0;
    SCL_P(PORT) = 1;

    /* SDA - input, pullup */
    SDA_P(DDR)  = 0;
    SDA_P(PORT) = 1;

    /* set bitrate */
    TWSR = 0;
    TWBR = (F_CPU / 100000UL - 16) / 2;
}

uint8_t twi_start(uint8_t mode)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
    return (TW_STATUS != mode);
}

uint8_t twi_addr(uint8_t addr, uint8_t mode)
{
    TWDR = (addr<<1) | mode;
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
    return TW_STATUS != (mode == TW_WRITE ? TW_MT_SLA_ACK : TW_MR_SLA_ACK);
}

uint8_t twi_transmit_data(uint8_t byte)
{
    TWDR = byte;
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
    return TW_STATUS != TW_MT_DATA_ACK;
}

int twi_receive_data()
{
    uint8_t data;
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
    data = TWDR;
    return data;
}

void twi_nack()
{
    TWCR = _BV(TWINT) |  _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
}

void twi_stop()
{
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}
