#ifndef __TWI_H__
#define __TWI_H__

#include <util/twi.h>

void twi_init();
uint8_t twi_start(uint8_t mode);
uint8_t twi_addr(uint8_t addr, uint8_t mode);
uint8_t twi_transmit_data(uint8_t byte);
int twi_receive_data();
void twi_stop();
void twi_nack();


#endif
