#ifndef USI_TWI_SLAVE_H
#define USI_TWI_SLAVE_H

#include <stdint.h>

void usi_twi_init(uint8_t address);

extern volatile uint8_t uts_rxCnt;
extern uint8_t          uts_rxBuf[];
extern volatile uint8_t uts_txBuf;

#endif /* USI_TWI_SLAVE_H */
