#include "usart.h"
#include "stdio.h"
//配置串口1 --ch340
//PA9 -- 复用推挽(既有高电平也有低电平)输出模式 --片上外设  PA10 -- 浮空输入模式
void usart_config(void)
{
	//1.设置GPIO口模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   //开启GPIOA时钟
	//PA9
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // 定义一个结构体变量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;    //PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);      //初始化PA9
	//PA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PB11

	//2.设置串口相关参数
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);	 //开启串口1时钟
	
	USART_InitTypeDef USART_InitStructure = {0}; //定义一个结构体
	
	USART_InitStructure.USART_BaudRate = 115200;         //波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //收发模式
	
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_Cmd(USART1, ENABLE);                //使能串口1 
	
}

// 发送单个字节函数
void usart_sendbit(uint8_t data)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != 1);
	USART_SendData(USART1, data);//发送一个字节的参数			 
}

// 发送多个字节数据
void usart_sendstr(uint8_t *str)
{
	while(*str != '\0')
	{
		usart_sendbit(*str);
		str++;
	}
}

// 接收函数
uint8_t receive(void)
{
	uint8_t data = 0;
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != 1); //等待接收数据
	data = USART_ReceiveData(USART1);
	return data;
}

// printf
int fputc(int ch, FILE *f)
{
	usart_sendbit(ch);
  return ch;
}
