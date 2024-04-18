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
	//发送命令 I2C模式连续读取指令
	IIC1_Start();
	printf("开始\r\n");
	if(IIC1_Send_Byte(0x54)==1)
		return 1;
	if(IIC1_Send_Byte(0x03)==1)
		return 1;
	if(IIC1_Send_Byte(0x00)==1)
		return 1;
	IIC1_Stop();
	delay_us(10);
	
	//接收数据
	IIC1_Start();
	printf("开始接收\r\n");

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
		printf("出错\r\n");
		return 2;//CRC校验出错
	}
	printf("准备校验数据\r\n");
	CO2Data = 0;//这里要清0,官网给出的程序未清0,（由于CO2Data是全局变量）,导致CO2浓度值非常高
	
//	CO2Data=(CO2Data|Byte[0])<<8;
//	CO2Data=(CO2Data|Byte[1])<<8;
//	CO2Data=(CO2Data|Byte[2])<<8;
//	CO2Data=(CO2Data|Byte[3]);
	
	CO2Data =( (Byte[0]<<24) | (Byte[1]<<16) |(Byte[2]<<8) |(Byte[3]) );
	printf("co2=%d\r\n",CO2Data);
	return 0;
}
