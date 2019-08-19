#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "gpio.h"
#include "usi_twi_slave.h"


#define LED(dir)   GPIO(B, 1, dir)

#define INIT_STATE_STORE  ((uint8_t*)1)

int main(void)
{
    // Load initial state and turn output on/off
    uint8_t ledstate = 0;
        LED(PORT) = PORT_LOW;

    LED(DDR) = DDR_OUT;

    usi_twi_init(0x30);

    sei();

    unsigned char addr = 0;

    uts_txBuf = 0xFF;

    for (;;)
    {
        while (uts_rxCnt == 0)
        {
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_cpu();
        }

        addr = uts_rxBuf[0];

        switch (addr)
        {
        // Register 0 - current state
        case 0x0:
            if (uts_rxCnt > 1)
            {
                ledstate = uts_rxBuf[1];
                if (ledstate)
                    LED(PORT) = PORT_HIGH;
                else
                    LED(PORT) = PORT_LOW;
                uts_rxCnt = 0;
            }
            uts_txBuf = ledstate;
            break;
        default:
            uts_txBuf = 0xFF;
        }
    }
}
