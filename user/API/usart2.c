#include "usart2.h"
#include "lora.h"
#include "stdio.h"
//PA2-TX-发送-复用推挽输出   PA3-RX-接收-浮空输入
void usart2_config(void)
{
	//1.设置GPIO口模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   //开启GPIOA时钟
	//PA2
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // 定义一个结构体变量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;    //PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);      //初始化PA2
	//PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
	
	//2.设置串口相关参数
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	 //开启串口2时钟
	
	USART_InitTypeDef USART_InitStructure = {0}; //定义一个结构体
	USART_InitStructure.USART_BaudRate = 9600;         //波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //收发模式	
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	//使能中断 -- 串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//使能空闲中断
	//设置中断优先级
	NVIC_InitTypeDef NVIC_InitStructure = {0};// 定义一个结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//次级优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//初始化
	
	USART_Cmd(USART2, ENABLE);                //使能串口2
}

// 发送单个字节函数
void u2_sendbit(uint8_t data)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != 1);
	USART_SendData(USART2, data);//发送一个字节的参数		
		USART_ClearFlag (USART2, USART_FLAG_TC);	
}

// 发送多个字节数据
void u2_sendstr(uint8_t *str,uint8_t len)
{
	for(uint8_t i=0;i<len;i++)
	{
		u2_sendbit(str[i]);
	}
}

// 发送多个字节数据
void u2_sendstr1(uint8_t *str,uint8_t len)
{
	uint8_t lo_buf[64] = {0x00,0x02,0x04};
	for(uint8_t i=0;i<len;i++)
	{
		lo_buf[i+3] = str[i];
		u2_sendbit(lo_buf[i+3]);
	}
}

uint8_t lora_receivebuf1[32]={0};
uint8_t lora_receivecnt1=0;
uint8_t lora_receiveflag1=0;
// 中断函数 -- 接收函数
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != 0)//发生了接收中断
	{
		uint8_t data =USART_ReceiveData(USART2);//接收数据赋给data -- 库函数
		lora_receivebuf1[lora_receivecnt1++] = data;
		if(lora_receivecnt1 == 20)
		{
			lora_receivecnt1 =0;
		}
		USART1 -> DR = data;//通过串口1把data发送出去
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != 0)//空闲中断
	{
		lora_receiveflag1 = 1;
		USART2 -> SR;
		USART2 -> DR;//清除空闲标志位
	}
}

