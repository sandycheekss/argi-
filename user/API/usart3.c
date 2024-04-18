#include "usart3.h"
#include "lora.h"
#include "stdio.h"
//PC10-TX-����-�����������   PC11-RX-����-��������
void usart3_config(void)
{
	//1.����GPIO��ģʽ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	   //����GPIOCʱ��
	//PC10
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // ����һ���ṹ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    //PC10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);      //��ʼ��PC10
	//PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC11
	
	//2.���ô�����ز���
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	 //��������3ʱ��
	
	USART_InitTypeDef USART_InitStructure = {0}; //����һ���ṹ��
	USART_InitStructure.USART_BaudRate = 9600;         //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //�շ�ģʽ	
	USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	//ʹ���ж� -- ����3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//ʹ�ܿ����ж�
	//�����ж����ȼ�
	NVIC_InitTypeDef NVIC_InitStructure = {0};// ����һ���ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�μ����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//��ʼ��
	
	USART_Cmd(USART3, ENABLE);                //ʹ�ܴ���3
}

// ���͵����ֽں���
void u3_sendbit(uint8_t data)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != 1);
	USART_SendData(USART3, data);//����һ���ֽڵĲ���			 
}

// ���Ͷ���ֽ�����
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
// �жϺ��� -- ���պ���
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != 0)//�����˽����ж�
	{
		uint8_t data =USART_ReceiveData(USART3);//�������ݸ���data -- �⺯��
		lora_receivebuf[lora_receivecnt++] = data;
		USART1 -> DR = data;//ͨ������1��data���ͳ�ȥ
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != 0)//�����ж�
	{
		lora_receiveflag = 1;
		USART3 -> SR;
		USART3 -> DR;//������б�־λ
	}
}

