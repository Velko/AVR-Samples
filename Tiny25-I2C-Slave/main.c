#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "usi_twi_slave.h"


#define SLAVE_ADDR_STORE  ((uint8_t*)0)
#define INIT_STATE_STORE  ((uint8_t*)1)

int main(void)
{
    // Load initial state and turn output on/off
    uint8_t ledstate = eeprom_read_byte(INIT_STATE_STORE);
    if (ledstate)
        PORTB |= 1 << PORTB1;
    else
        PORTB &= ~(1 << PORTB1);
    DDRB |= 1 << PORTB1;

    uts_init();
    uts_slaveAddress = eeprom_read_byte(SLAVE_ADDR_STORE);

    // reset to default, if invalid address
    if (uts_slaveAddress < 8 || uts_slaveAddress >= 120)
        uts_slaveAddress = 0x30;

    sei();

    unsigned char addr = 0;

    uts_txBuf = 0xFF;

    for (;;)
    {
        if (uts_rxCnt > 0) {
            addr = uts_rxBuf[0];

            switch (addr)
            {
            // Register 0 - current state
            case 0x0:
                if (uts_rxCnt > 1)
                {
                    ledstate = uts_rxBuf[1];
                    if (ledstate)
                        PORTB |= 1 << PORTB1;
                    else
                        PORTB &= ~(1 << PORTB1);
                }
                uts_txBuf = ledstate;
                break;
            // Register 0x10 - device address
            case 0x10:
                if (uts_rxCnt > 1)
                {
                    // set and store slave address
                    uts_slaveAddress = uts_rxBuf[1];
                    if (uts_slaveAddress < 8 || uts_slaveAddress >= 120)
                        uts_slaveAddress = 0x30;
                    eeprom_write_byte(SLAVE_ADDR_STORE, uts_slaveAddress);
                }
                uts_txBuf = uts_slaveAddress;
                break;
            // Register 0x11 - startup state
            case 0x11:
                if (uts_rxCnt > 1)
                {
                    eeprom_write_byte(INIT_STATE_STORE, uts_rxBuf[1]);
                }
                uts_txBuf = eeprom_read_byte(INIT_STATE_STORE);
                break;
            default:
                uts_txBuf = 0xFF;
            }
        }
    }
}
