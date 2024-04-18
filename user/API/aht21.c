#include "aht21.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"

uint8_t AHT21_State=0;   //0表示发送命令  1表示采集状态
uint8_t AHT21_Send_Cmd[3]={0xac,0x33,0x00};
uint8_t AHT21_Receive_Date[7];
uint8_t AHT21_CRC=0;

AHT21_DATE  AHT21_Date;

//初始化IIC接口
void AHT21_Init(void)
{
	AHT21_I2C_Config();
}

//IIC引脚初始化  PC4 PC5 开漏输出
//PC4  SCL
//PC5  SDA
void AHT21_I2C_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_4|GPIO_Pin_5);	
}

void AHT21_I2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOC, GPIO_Pin_4, (BitAction)BitValue);
	delay_us(10);
}

void AHT21_I2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOC, GPIO_Pin_5, (BitAction)BitValue);
	delay_us(10);
}

uint8_t AHT21_I2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
	delay_us(10);
	return BitValue;
}

//除了停止信号  其他的结束SCL都为低电平
//函数：起始条件	当SCL为高电平，SDA由高到低
void AHT21_I2C_Start(void)
{
	AHT21_I2C_W_SDA(1);   //保险起见先释放SDA,如果原来是高也不影响
	AHT21_I2C_W_SCL(1);
	AHT21_I2C_W_SDA(0);
	AHT21_I2C_W_SCL(0);
}

//函数：终止条件	当SCL为高电平，SDA由低到高
void AHT21_I2C_Stop(void)
{
	AHT21_I2C_W_SDA(0);
	AHT21_I2C_W_SCL(1);
	AHT21_I2C_W_SDA(1);
}

//函数：发送一个字节	SCL低，主机将数据位放到SDA(由高到低)，拉高
//								SCL高，从机在SCL高时读取数据位，循环八次，发送一个字节
void AHT21_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		AHT21_I2C_W_SDA(Byte & (0x80 >> i));
		AHT21_I2C_W_SCL(1);   //释放SCL后，从机会立即读取数据位
		AHT21_I2C_W_SCL(0);
	}
}

//函数：接收一个字节	SCL低，从机将数据位放到SDA(由高到低)，拉高
//								SCL高，主机在SCL高时读取数据位，循环八次，接收一个字节（主机接收之前需要释放SDA）
uint8_t AHT21_I2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	AHT21_I2C_W_SDA(1);   //切换成输入状态,主机释放SDA，防止从机干扰
	for (i = 0; i < 8; i ++)
	{
		AHT21_I2C_W_SCL(1);  //释放SCL后，主机会立即读取数据位
		if (AHT21_I2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		AHT21_I2C_W_SCL(0);//SCL读取数据位后，拉低，之后从机就会将下一个数据位放到SDA，循环8次
	}
	return Byte;
}

//数据1  非应答
//数据0  应答
//函数：发送应答	SCL低，主机将应答位放到SDA(由高到低)，拉高
//								SCL高，从机在SCL高时读取应答位，发送一位
void AHT21_I2C_SendAck(uint8_t AckBit)
{
			//函数进来SCL默认为低电平
	AHT21_I2C_W_SDA(AckBit);
	AHT21_I2C_W_SCL(1);  //释放SCL后，从机会立即读取数据位
	AHT21_I2C_W_SCL(0);
}

//函数：接收应答	SCL低，从机将应答位放到SDA(由高到低)，拉高
//								SCL高，主机在SCL高时读取应答位，接收1位（主机接收之前需要释放SDA）
uint8_t AHT21_I2C_ReceiveAck(void)
{
			//函数进来SCL默认为低电平
	uint8_t AckBit;
	AHT21_I2C_W_SDA(1);   //切换SDA为输入状态,//主机释放SDA，防止从机干扰
	AHT21_I2C_W_SCL(1);   //释放SCL后，主机会立即读取数据位
	AckBit = AHT21_I2C_R_SDA();
	AHT21_I2C_W_SCL(0);//SCL读取数据位后，拉低，之后从机就会将下一个数据位放到应答位，进入下一个时序单元
	return AckBit;
}

void AHT21_Read_Continuous(u8 *pBuffer,u16 NumToRead)
{ 	    																 
		AHT21_I2C_Start();  
		AHT21_I2C_SendByte(0X71);   //发送器件地址0X71,写数据 	 
		AHT21_I2C_ReceiveAck(); 
		while(NumToRead--)
		{
			*pBuffer++=AHT21_I2C_ReceiveByte();
			if(NumToRead==0)  //接收完毕,回复NACK
				AHT21_I2C_SendAck(1);
			else
				AHT21_I2C_SendAck(0);	//未接收完成,回复ACK
		}
		AHT21_I2C_Stop();//产生一个停止条件	    
}  

void AHT21_Write_Continuous(u8 *pBuffer,u16 NumToWrite)
{
		AHT21_I2C_Start();  
		AHT21_I2C_SendByte(0X70);   //发送器件地址0X70,写数据 
		AHT21_I2C_ReceiveAck();	   
		while(NumToWrite--)
		{		
			AHT21_I2C_SendByte(*pBuffer);     //发送字节							   
			AHT21_I2C_ReceiveAck();
			pBuffer++;			
		}   	   
    AHT21_I2C_Stop();//产生一个停止条件 
}

 /*******************************************************************************
* Function Name  : CheckCrc
* Description    :  
* Input          :  None
* Output         : None
* Return         :None
*******************************************************************************/
unsigned char  CheckCrc8(unsigned char *pDat,unsigned char Lenth)
{
unsigned char crc = 0xff, i, j;

    for (i = 0; i < Lenth ; i++)
    {
        crc = crc ^ *pDat;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80) crc = (crc << 1) ^ 0x31;
            else crc <<= 1;
        }
				pDat++;
    }
    return crc;
}


void AHT21_Handle(void)
{
	if(AHT21_State==0)
	{
		AHT21_Write_Continuous(AHT21_Send_Cmd,3);
		AHT21_State=1;
	}
	else if(AHT21_State==1)
	{
		AHT21_State=0;
		memset(AHT21_Receive_Date,0,sizeof(AHT21_Receive_Date));
		AHT21_Read_Continuous(AHT21_Receive_Date,7);
		AHT21_CRC=CheckCrc8(AHT21_Receive_Date,6);
		if(AHT21_CRC==AHT21_Receive_Date[6])
		{
			AHT21_Date.Hum_Date=AHT21_Receive_Date[1];
			AHT21_Date.Hum_Date=AHT21_Date.Hum_Date<<8;AHT21_Date.Hum_Date+=AHT21_Receive_Date[2];
			AHT21_Date.Hum_Date=AHT21_Date.Hum_Date<<8;AHT21_Date.Hum_Date+=AHT21_Receive_Date[3];
			AHT21_Date.Hum_Date=AHT21_Date.Hum_Date>>4;	
			AHT21_Date.Hum=AHT21_Date.Hum_Date*100.0/1048576;
			AHT21_Date.Tem_Date=AHT21_Receive_Date[3]&0x0F;
			AHT21_Date.Tem_Date=AHT21_Date.Tem_Date<<8;AHT21_Date.Tem_Date+=AHT21_Receive_Date[4];
			AHT21_Date.Tem_Date=AHT21_Date.Tem_Date<<8;AHT21_Date.Tem_Date+=AHT21_Receive_Date[5];		
			AHT21_Date.Tem=(AHT21_Date.Tem_Date*200.0)/1048576-50.0;		
			printf("AHT21_Date.Hum=%.1f\r\n",AHT21_Date.Hum);
			printf("AHT21_Date.Tem=%.1f\r\n",AHT21_Date.Tem);
		}
		else
		{
			printf("校验不通过\r\n");
		}
	}	
}
