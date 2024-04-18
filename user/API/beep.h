#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"


void beep_config(void);
void beepon(void);
void beepoff(void);

extern uint32_t BEEPtime;

#endif
