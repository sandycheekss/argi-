#include "iic1.h"
#include "delay.h"
#include "stdio.h"
//I2C��ʼ��
//I2Cͨ�� PC3 -- SCL   PC13 -- SDA
void I2C_config1(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0}; // ����һ���ṹ�����
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_OD; //��©���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);      //��ʼ��PB
//	GPIO_SetBits(GPIOC,GPIO_Pin_3|GPIO_Pin_13);
	
	//�����ڿ���״̬
	IIC1_SCL_OUT1;  //1Ϊ�� 0Ϊ��
	IIC1_SDA_OUT1;
}

//��ʼ�ź�
void IIC1_Start(void)
{
	//Ӳ������ ��ʼSCLΪ��
	IIC1_SCL_OUT0;
	delay_us(6);
	//1.SCL SDA �ø�
	IIC1_SDA_OUT1;
	IIC1_SCL_OUT1;
	//2.��ʱ�� SCL SDA�õ�
	delay_us(6);//��ʱ---��ʼ�����Ľ���ʱ��
	IIC1_SDA_OUT0;//---������ʼ����
	delay_us(6);//��ʱ---��ʼ�����ı���ʱ��
	IIC1_SCL_OUT0; //---������ʼ����  ׼������/��������
}

//�����ź�
void IIC1_Stop(void)
{
	//1.������SDA��������SCL
	IIC1_SDA_OUT0;
	IIC1_SCL_OUT0;
	//2.����SCL
	IIC1_SCL_OUT1;
	delay_us(6);//��ʱ---ֹͣ�����Ľ���ʱ��
	//3.����SDA
	IIC1_SDA_OUT1; //---������ֹͣ����
	delay_us(6);//��ʱ---����ͨ�Ž������´�ͨ�ſ�ʼ��ʱ��

}

//��������Ӧ���ź�
void IIC1_Send_Ack(uint8_t ack)
{
	IIC1_SCL_OUT0;
	if(ack)//(����׼������)
		IIC1_SDA_OUT1; //��Ӧ��
	else
		IIC1_SDA_OUT0; //��Ӧ��
	delay_us(6);//��ʱ�������ȶ����������ϣ�
	IIC1_SCL_OUT1;//���ӻ���ʱ���������ش�SDA�ϲɼ����ݣ�
	delay_us(6);//��ʱ����ʱ��ӻ���ȡ���ݣ�
	
	IIC1_SCL_OUT0;//��������Ĳ�������ֹ���������ֹͣ����
}

//������ȡӦ���ź�    0--��Ӧ��  1--��Ӧ��
uint8_t IIC1_Revice_Ack(void)
{
	uint8_t ack=0;
	//1.�ӻ�׼������
	IIC1_SCL_OUT0;//���ӻ�׼�����ݣ�
	//2.SDA SCL ����
	IIC1_SDA_OUT1;//��ģʽ-----�������·��ܽŶϿ���������������������
	delay_us(6);//��ʱ����ʱ��ӻ�׼�����ݲ��������ȶ����������ϣ�
	IIC1_SCL_OUT1;
	delay_us(6);//��ʱ (��ʱ��������ȡ����)
	//3.��ȡSDA���� 1--�ߵ�ƽ--��Ӧ��  0 --�͵�ƽ -- Ӧ��
	if(IIC1_SDA_IN)//������ȡSDA���ϵ�����
		ack=1;
	
	//IIC1_SCL=0;//��������Ĳ�������ֹ���������ֹͣ����
	return ack;
}

//��������һ���ֽڸ��ӻ����Ҷ�ȡһ��Ӧ���ź�  Ӧ���ź� 0--��Ӧ��  1--��Ӧ��
uint8_t IIC1_Send_Byte(uint8_t data)
{
	uint8_t i=0;

	for(i=0;i<8;i++)
	{
		IIC1_SCL_OUT0;
		if((data<<i)&0x80)//(����׼������)  
			IIC1_SDA_OUT1;           
		else
			IIC1_SDA_OUT0;
		delay_us(6);//��ʱ�������ȶ����������ϣ�
		IIC1_SCL_OUT1;//���ӻ���ʱ���������ش�SDA�ϲɼ����ݣ�
		delay_us(6);//��ʱ����ʱ��ӻ���ȡ���ݣ�
	}
	//IIC1_SCL=0;//��������Ĳ�������ֹ���������ֹͣ����
	
	return IIC1_Revice_Ack();
}

//������ȡһ���ֽڲ��ҷ���һ��Ӧ���ź�   Ӧ���ź� 0--��Ӧ��  1--��Ӧ��
uint8_t IIC1_Revice_Byte(uint8_t ack)
{
	uint8_t i=0;
	uint8_t data=0;
	
	for(i=0;i<8;i++)
	{
		IIC1_SCL_OUT0;//���ӻ�׼�����ݣ�
		IIC1_SDA_OUT1;//��ģʽ-----�������·��ܽŶϿ���������������������
		delay_us(6);//��ʱ����ʱ��ӻ�׼�����ݲ��������ȶ����������ϣ�
		IIC1_SCL_OUT1;
		data = (data<<1) | (IIC1_SDA_IN);
		
		delay_us(6);//��ʱ (��ʱ��������ȡ����)
		
	}
	//IIC1_SCL=0;//��������Ĳ�������ֹ���������ֹͣ����
	delay_us(6);
	IIC1_Send_Ack(ack);
	delay_us(6);
	return data;
}

//У�鷽ʽ
uint8_t Calc_CRC8(uint8_t *dat, uint8_t Num)
{
	uint8_t i,byte,crc=0xFF;
	for(byte=0; byte<Num; byte++)
	{
		crc^=(dat[byte]);
		for(i=0;i<8;i++)
		{
			if(crc & 0x80) 
				crc=(crc<<1)^0x31;
			else 
				crc=(crc<<1);
		}
	}
	return crc;
}
