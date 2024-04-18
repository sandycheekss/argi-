#include "led.h"

void led_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //����(ʹ��)GPIOCʱ��
	
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // ����һ���ṹ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2; // LED-->PC0.1.2 �˿�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // ��ͨ�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //�����趨������ʼ��GPIOE2.3.4.5
	
	GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);//����Ϊ�ߵ�ƽ �ص�
	//GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);//����Ϊ�͵�ƽ ����
	
}

//����
void ledon(GPIO_TypeDef * ledport,uint16_t ledpin)
{
	GPIO_ResetBits(ledport,ledpin);
}

//�ص�
void ledoff(GPIO_TypeDef * ledport,uint16_t ledpin)
{
	GPIO_SetBits(ledport,ledpin);
}

//��ת
void ledturn(GPIO_TypeDef * ledport,uint16_t ledpin)
{
	if(GPIO_ReadOutputDataBit(ledport, ledpin) == 1)
		GPIO_ResetBits(ledport,ledpin);//����
	else
		GPIO_SetBits(ledport,ledpin);//�ص�
}
