#include "beep.h"

void beep_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //����(ʹ��)GPIOCʱ��
	
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // ����һ���ṹ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // BEEP-->PC0 �˿�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // ��ͨ�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //�����趨������ʼ��GPIOC0
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);//����Ϊ�͵�ƽ �ر�

}

//����������
void beepon(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
}

//�رշ�����
void beepoff(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}
