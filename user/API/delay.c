#include "delay.h"

uint32_t ledtime[2] ={0,1000};
uint32_t keytime[2] ={0,1000};
uint32_t co2time[2] ={0,1000};
uint32_t loratime[2] ={0,1000};
uint32_t ahttime[2] ={0,1000};
uint32_t lighttime[2] ={0,1000};
uint32_t bmptime[2] ={0,1000};
void delay_1us(void)
{
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();
	
}

//us��ʱ
void delay_us(uint16_t t)
{
	for(uint16_t i=0;i<t;i++)
	{
		delay_1us();
	}

}

//ms��ʱ
void delay_ms(uint32_t t)
{
	t = t * 1000;
	for(uint32_t i=0;i<t;i++)
	{
		delay_1us();
	}

}

void SysTick_Handler(void)//ϵͳ��ʱ���ж�
{
	ledtime[0]++;
	keytime[0]++;
	co2time[0]++;
	loratime[0]++;
	ahttime[0]++;
	lighttime[0]++;
	bmptime[0]++;
}
