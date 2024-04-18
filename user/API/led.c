#include "led.h"

void led_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //开启(使能)GPIOC时钟
	
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // 定义一个结构体变量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2; // LED-->PC0.1.2 端口配置
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // IO口速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 普通推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //根据设定参数初始化GPIOE2.3.4.5
	
	GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);//设置为高电平 关灯
	//GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);//设置为低电平 开灯
	
}

//开灯
void ledon(GPIO_TypeDef * ledport,uint16_t ledpin)
{
	GPIO_ResetBits(ledport,ledpin);
}

//关灯
void ledoff(GPIO_TypeDef * ledport,uint16_t ledpin)
{
	GPIO_SetBits(ledport,ledpin);
}

//反转
void ledturn(GPIO_TypeDef * ledport,uint16_t ledpin)
{
	if(GPIO_ReadOutputDataBit(ledport, ledpin) == 1)
		GPIO_ResetBits(ledport,ledpin);//开灯
	else
		GPIO_SetBits(ledport,ledpin);//关灯
}
