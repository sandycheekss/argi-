#include "usart3.h"
#include "lora.h"
#include "stdio.h"
//PC10-TX-发送-复用推挽输出   PC11-RX-接收-浮空输入
void usart3_config(void)
{
	//1.设置GPIO口模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	   //开启GPIOC时钟
	//PC10
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // 定义一个结构体变量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    //PC10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);      //初始化PC10
	//PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11
	
	//2.设置串口相关参数
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	 //开启串口3时钟
	
	USART_InitTypeDef USART_InitStructure = {0}; //定义一个结构体
	USART_InitStructure.USART_BaudRate = 9600;         //波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //收发模式	
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	//使能中断 -- 串口3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//使能空闲中断
	//设置中断优先级
	NVIC_InitTypeDef NVIC_InitStructure = {0};// 定义一个结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//次级优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//初始化
	
	USART_Cmd(USART3, ENABLE);                //使能串口3
}

// 发送单个字节函数
void u3_sendbit(uint8_t data)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != 1);
	USART_SendData(USART3, data);//发送一个字节的参数			 
}

// 发送多个字节数据
void u3_sendstr(uint8_t *str,uint8_t len)
{
	for(uint8_t i=0;i<len;i++)
	{
		u3_sendbit(str[i]);
	}
}

uint8_t lora_receivebuf[32]={0};
uint8_t lora_receivecnt=0;
uint8_t lora_receiveflag=0;
// 中断函数 -- 接收函数
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != 0)//发生了接收中断
	{
		uint8_t data =USART_ReceiveData(USART3);//接收数据赋给data -- 库函数
		lora_receivebuf[lora_receivecnt++] = data;
		USART1 -> DR = data;//通过串口1把data发送出去
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != 0)//空闲中断
	{
		lora_receiveflag = 1;
		USART3 -> SR;
		USART3 -> DR;//清除空闲标志位
	}
}

