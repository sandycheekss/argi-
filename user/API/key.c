#include "key.h"
#include "delay.h"

#include "usart2.h"
void key_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB ,ENABLE); //����(ʹ��)GPIOAʱ��
	//GPIOA
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // ����һ���ṹ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // KEY-->PA0�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //�����趨������ʼ��GPIOA0
	//GPIOC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // KEY-->PA4.5.6�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // ��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //�����趨������ʼ��GPIOC4.5.6
	
	//2.��������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�������ù���ʱ�� -- RCC
	//EXITLineConfig -- ѡ��GPIO�ܿ������ⲿ�ж���·
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);//����GPIO�� PA0 -- EXIT0 -- **�ж���** -- ���ж���EXTI0��ӦPA0
	//3.EXTI -- �ⲿ�жϿ����� 
	EXTI_InitTypeDef EXTI_InitStructure = {0}; //������һ���ṹ�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; //�ж���0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //�жϴ���
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //����/�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; //ʹ���ж�
	EXTI_Init(&EXTI_InitStructure);//��ʼ��
	//4.NVIC -- �жϿ����� -- �����ж����ȼ�
	NVIC_InitTypeDef NVIC_InitStruct = {0};// ����һ���ṹ�����
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; //ͨ��  **�жϺ�**
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ� (0-3 �������) 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; //�μ����ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_Init(&NVIC_InitStruct);//��ʼ��
}

uint8_t key_flag=0;
uint8_t key_value(void)
{
	//����1 PA0
	uint8_t value = 0;
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)//��һ�μ�⵽��������(�˿�����)
	{
		delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)//�ڶ��μ�⵽��������
		{
			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)//�ȴ�����̧��
			value = 1;
		}
	}

	//����2 PB8
	if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 0)//��һ�μ�⵽��������(�˿�����)
	{
		delay_ms(10);
		if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 0)//�ڶ��μ�⵽��������
		{
			while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 0)//�ȴ�����̧��
			value = 2;
		}
	}
	
	return value;
	
}

//�жϷ�����
uint8_t keyflag=0;
void EXTI0_IRQHandler(void)//�����ļ�
{
	if( EXTI_GetITStatus(EXTI_Line0) != 0)//�������ж�
	{
		keyflag = 1;
		EXTI_ClearITPendingBit(EXTI_Line0);//����жϱ���λ
	}
}
