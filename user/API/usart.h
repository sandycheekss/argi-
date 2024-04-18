#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

void usart_config(void);
void usart_sendbit(uint8_t data);
void usart_sendstr(uint8_t *str);

uint8_t receive(void);

#endif
