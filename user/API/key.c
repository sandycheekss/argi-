#include "key.h"
#include "delay.h"

#include "usart2.h"
void key_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB ,ENABLE); //开启(使能)GPIOA时钟
	//GPIOA
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // 定义一个结构体变量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // KEY-->PA0端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //根据设定参数初始化GPIOA0
	//GPIOC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // KEY-->PA4.5.6端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // 浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //根据设定参数初始化GPIOC4.5.6
	
	//2.开启复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//开启复用功能时钟 -- RCC
	//EXITLineConfig -- 选择GPIO管口用作外部中断线路
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);//配置GPIO口 PA0 -- EXIT0 -- **中断线** -- 将中断线EXTI0对应PA0
	//3.EXTI -- 外部中断控制器 
	EXTI_InitTypeDef EXTI_InitStructure = {0}; //定义了一个结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; //中断线0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //中断触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升/下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; //使能中断
	EXTI_Init(&EXTI_InitStructure);//初始化
	//4.NVIC -- 中断控制器 -- 设置中断优先级
	NVIC_InitTypeDef NVIC_InitStruct = {0};// 定义一个结构体变量
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; //通道  **中断号**
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级 (0-3 随便设置) 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; //次级优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_Init(&NVIC_InitStruct);//初始化
}

uint8_t key_flag=0;
uint8_t key_value(void)
{
	//按键1 PA0
	uint8_t value = 0;
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)//第一次检测到按键按下(端口输入)
	{
		delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)//第二次检测到按键按下
		{
			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)//等待按键抬起
			value = 1;
		}
	}

	//按键2 PB8
	if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 0)//第一次检测到按键按下(端口输入)
	{
		delay_ms(10);
		if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 0)//第二次检测到按键按下
		{
			while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 0)//等待按键抬起
			value = 2;
		}
	}
	
	return value;
	
}

//中断服务函数
uint8_t keyflag=0;
void EXTI0_IRQHandler(void)//启动文件
{
	if( EXTI_GetITStatus(EXTI_Line0) != 0)//发生了中断
	{
		keyflag = 1;
		EXTI_ClearITPendingBit(EXTI_Line0);//清除中断标制位
	}
}
