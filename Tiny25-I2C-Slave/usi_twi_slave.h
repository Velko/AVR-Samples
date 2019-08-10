#ifndef USI_TWI_SLAVE_H
#define USI_TWI_SLAVE_H

void usi_twi_init(void);

typedef unsigned char uint8_t;

extern volatile uint8_t uts_rxCnt;
extern uint8_t          uts_rxBuf[];
extern volatile uint8_t uts_txBuf;

extern uint8_t twi_slaveAddress;

#endif /* USI_TWI_SLAVE_H */
