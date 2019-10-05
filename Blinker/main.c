#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h>

#include "gpio.h"

#define LED(dir)    GPIO(B, 4, dir)

int main(void)
{
    LED(PORT) = PORT_LOW;
    LED(DDR) = DDR_OUT;

    for (;;)
    {
        _delay_ms(500);
        LED(PORT) = !LED(PORT);
    }
}
