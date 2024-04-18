#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

extern uint32_t keytime[2];
uint8_t key_value(void);
void key_config(void);

extern uint8_t keyflag;

#endif
