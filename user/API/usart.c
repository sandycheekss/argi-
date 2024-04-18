#include "usart.h"
#include "stdio.h"
//���ô���1 --ch340
//PA9 -- ��������(���иߵ�ƽҲ�е͵�ƽ)���ģʽ --Ƭ������  PA10 -- ��������ģʽ
void usart_config(void)
{
	//1.����GPIO��ģʽ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   //����GPIOAʱ��
	//PA9
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // ����һ���ṹ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;    //PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);      //��ʼ��PA9
	//PA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PB11

	//2.���ô�����ز���
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);	 //��������1ʱ��
	
	USART_InitTypeDef USART_InitStructure = {0}; //����һ���ṹ��
	
	USART_InitStructure.USART_BaudRate = 115200;         //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //�շ�ģʽ
	
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_Cmd(USART1, ENABLE);                //ʹ�ܴ���1 
	
}

// ���͵����ֽں���
void usart_sendbit(uint8_t data)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != 1);
	USART_SendData(USART1, data);//����һ���ֽڵĲ���			 
}

// ���Ͷ���ֽ�����
void usart_sendstr(uint8_t *str)
{
	while(*str != '\0')
	{
		usart_sendbit(*str);
		str++;
	}
}

// ���պ���
uint8_t receive(void)
{
	uint8_t data = 0;
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != 1); //�ȴ���������
	data = USART_ReceiveData(USART1);
	return data;
}

// printf
int fputc(int ch, FILE *f)
{
	usart_sendbit(ch);
  return ch;
}
