#ifndef __DELAY_H
#define __DELAY_H 		

#include "stm32f10x.h" 
	 
void delay_us(uint16_t t);
void delay_ms(uint32_t t);
void delay_1us(void);

extern uint32_t ledtime[2];
#endif
