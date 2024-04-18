#include "usart2.h"
#include "lora.h"
#include "stdio.h"
//PA2-TX-����-�����������   PA3-RX-����-��������
void usart2_config(void)
{
	//1.����GPIO��ģʽ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   //����GPIOAʱ��
	//PA2
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // ����һ���ṹ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;    //PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);      //��ʼ��PA2
	//PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
	
	//2.���ô�����ز���
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	 //��������2ʱ��
	
	USART_InitTypeDef USART_InitStructure = {0}; //����һ���ṹ��
	USART_InitStructure.USART_BaudRate = 9600;         //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //�շ�ģʽ	
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	//ʹ���ж� -- ����2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//ʹ�ܿ����ж�
	//�����ж����ȼ�
	NVIC_InitTypeDef NVIC_InitStructure = {0};// ����һ���ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�μ����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//��ʼ��
	
	USART_Cmd(USART2, ENABLE);                //ʹ�ܴ���2
}

// ���͵����ֽں���
void u2_sendbit(uint8_t data)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != 1);
	USART_SendData(USART2, data);//����һ���ֽڵĲ���		
		USART_ClearFlag (USART2, USART_FLAG_TC);	
}

// ���Ͷ���ֽ�����
void u2_sendstr(uint8_t *str,uint8_t len)
{
	for(uint8_t i=0;i<len;i++)
	{
		u2_sendbit(str[i]);
	}
}

// ���Ͷ���ֽ�����
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
// �жϺ��� -- ���պ���
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != 0)//�����˽����ж�
	{
		uint8_t data =USART_ReceiveData(USART2);//�������ݸ���data -- �⺯��
		lora_receivebuf1[lora_receivecnt1++] = data;
		if(lora_receivecnt1 == 20)
		{
			lora_receivecnt1 =0;
		}
		USART1 -> DR = data;//ͨ������1��data���ͳ�ȥ
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != 0)//�����ж�
	{
		lora_receiveflag1 = 1;
		USART2 -> SR;
		USART2 -> DR;//������б�־λ
	}
}

