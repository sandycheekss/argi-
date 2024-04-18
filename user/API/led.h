#ifndef __LED_H
#define __LED_H 		

#include "stm32f10x.h" 

#define led1port  GPIOC
#define led2port  GPIOC
#define led3port  GPIOC

#define led1pin   GPIO_Pin_0
#define led2pin   GPIO_Pin_1
#define led3pin   GPIO_Pin_2


void led_config(void);
void ledon(GPIO_TypeDef * ledport,uint16_t ledpin);
void ledoff(GPIO_TypeDef * ledport,uint16_t ledpin);
void ledturn(GPIO_TypeDef * ledport,uint16_t ledpin);

extern uint32_t ledtime[2];

#endif
