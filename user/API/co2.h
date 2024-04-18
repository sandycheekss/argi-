#ifndef __CO2_H
#define __CO2_H

#include "stm32f10x.h"

extern uint32_t co2time[2];

uint32_t ACD10_Read_Data(void);

void co2_sendcmd(void);
void co2_receive(uint8_t *p,uint16_t Num);
void co2_analysis(void);

uint32_t CO2_Read_Data(void);
#endif
