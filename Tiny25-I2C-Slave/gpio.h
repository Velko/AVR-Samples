/* 
 * General purpose IO pin mapping.
 *
 * (c) Jurgis Brigmanis 2008
 */

/* usage (LED connected to PB0): 
 *  #define LED(dir)   GPIO(B, 0, dir)
 *  
 *  LED(DDR) = 1;
 *  LED(PORT) = 1;
*/

#ifndef __GPIO_H__
#define __GPIO_H__

struct bits {
  uint8_t b0:1;
  uint8_t b1:1;
  uint8_t b2:1;
  uint8_t b3:1;
  uint8_t b4:1;
  uint8_t b5:1;
  uint8_t b6:1;
  uint8_t b7:1;
} __attribute__((__packed__)); 


#define GPIO(name,pin,dir) \
 ((*(volatile struct bits*)&dir##name).b##pin)

#endif
