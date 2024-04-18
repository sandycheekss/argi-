#include "co2.h"
#include "stdio.h"
#include "delay.h"
#include "iic1.h"

uint16_t CO2Data=0;
uint8_t Byte[6]={0},CRC_Byte[3]={0};

uint32_t CO2_Read_Data(void)
{
	CO2Data = 0;
//	I2C_config1();
	//�������� I2Cģʽ������ȡָ��
	IIC1_Start();
	printf("��ʼ\r\n");
	if(IIC1_Send_Byte(0x54)==1)
		return 1;
	if(IIC1_Send_Byte(0x03)==1)
		return 1;
	if(IIC1_Send_Byte(0x00)==1)
		return 1;
	IIC1_Stop();
	delay_us(10);
	
	//��������
	IIC1_Start();
	printf("��ʼ����\r\n");

	if(IIC1_Send_Byte(0x55)==1)
		return 1;
	
	Byte[0] = IIC1_Revice_Byte(0);
	Byte[1] = IIC1_Revice_Byte(0);
	CRC_Byte[0] = IIC1_Revice_Byte(0);

//	printf("Byte[0]=%d\r\n",Byte[0]);
//	printf("Byte[1]=%d\r\n",Byte[1]);
//	printf("CRC_Byte[0]=%d\r\n",CRC_Byte[0]);
	
	Byte[2] = IIC1_Revice_Byte(0);
	Byte[3] = IIC1_Revice_Byte(0);
	CRC_Byte[1] = IIC1_Revice_Byte(0);
	
//	printf("Byte[2]=%d\r\n",Byte[2]);
//	printf("Byte[3]=%d\r\n",Byte[3]);
//	printf("CRC_Byte[1]=%d\r\n",CRC_Byte[1]);
	
	Byte[4] = IIC1_Revice_Byte(0);
	Byte[5] = IIC1_Revice_Byte(0);
	CRC_Byte[2] = IIC1_Revice_Byte(1);
	
//	printf("Byte[4]=%d\r\n",Byte[4]);
//	printf("Byte[5]=%d\r\n",Byte[5]);
//	printf("CRC_Byte[2]=%d\r\n",CRC_Byte[2]);
	
	IIC1_Stop();
	
	if((Calc_CRC8(Byte,2)!=CRC_Byte[0])||(Calc_CRC8(Byte+2,2)!=CRC_Byte[1])||(Calc_CRC8(Byte+4,2)!=CRC_Byte[2]))
	{
		printf("����\r\n");
		return 2;//CRCУ�����
	}
	printf("׼��У������\r\n");
	CO2Data = 0;//����Ҫ��0,���������ĳ���δ��0,������CO2Data��ȫ�ֱ�����,����CO2Ũ��ֵ�ǳ���
	
//	CO2Data=(CO2Data|Byte[0])<<8;
//	CO2Data=(CO2Data|Byte[1])<<8;
//	CO2Data=(CO2Data|Byte[2])<<8;
//	CO2Data=(CO2Data|Byte[3]);
	
	CO2Data =( (Byte[0]<<24) | (Byte[1]<<16) |(Byte[2]<<8) |(Byte[3]) );
	printf("co2=%d\r\n",CO2Data);
	return 0;
}
