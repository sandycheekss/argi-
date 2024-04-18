#ifndef __USART3_H
#define __USART3_H

#include "stm32f10x.h"

void usart3_config(void);
void u3_sendbit(uint8_t data);
void u3_sendstr(uint8_t *str,uint8_t len);

void lora_Rec(uint8_t *data,uint8_t len);

#endif
