#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"

void usart2_config(void);
void u2_sendbit(uint8_t data);
void u2_sendstr(uint8_t *str,uint8_t len);
void u2_sendstr1(uint8_t *str,uint8_t len);


#endif
