#include "aht21.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"

uint8_t AHT21_State=0;   //0��ʾ��������  1��ʾ�ɼ�״̬
uint8_t AHT21_Send_Cmd[3]={0xac,0x33,0x00};
uint8_t AHT21_Receive_Date[7];
uint8_t AHT21_CRC=0;

AHT21_DATE  AHT21_Date;

//��ʼ��IIC�ӿ�
void AHT21_Init(void)
{
	AHT21_I2C_Config();
}

//IIC���ų�ʼ��  PC4 PC5 ��©���
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

//����ֹͣ�ź�  �����Ľ���SCL��Ϊ�͵�ƽ
//��������ʼ����	��SCLΪ�ߵ�ƽ��SDA�ɸߵ���
void AHT21_I2C_Start(void)
{
	AHT21_I2C_W_SDA(1);   //����������ͷ�SDA,���ԭ���Ǹ�Ҳ��Ӱ��
	AHT21_I2C_W_SCL(1);
	AHT21_I2C_W_SDA(0);
	AHT21_I2C_W_SCL(0);
}

//��������ֹ����	��SCLΪ�ߵ�ƽ��SDA�ɵ͵���
void AHT21_I2C_Stop(void)
{
	AHT21_I2C_W_SDA(0);
	AHT21_I2C_W_SCL(1);
	AHT21_I2C_W_SDA(1);
}

//����������һ���ֽ�	SCL�ͣ�����������λ�ŵ�SDA(�ɸߵ���)������
//								SCL�ߣ��ӻ���SCL��ʱ��ȡ����λ��ѭ���˴Σ�����һ���ֽ�
void AHT21_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		AHT21_I2C_W_SDA(Byte & (0x80 >> i));
		AHT21_I2C_W_SCL(1);   //�ͷ�SCL�󣬴ӻ���������ȡ����λ
		AHT21_I2C_W_SCL(0);
	}
}

//����������һ���ֽ�	SCL�ͣ��ӻ�������λ�ŵ�SDA(�ɸߵ���)������
//								SCL�ߣ�������SCL��ʱ��ȡ����λ��ѭ���˴Σ�����һ���ֽڣ���������֮ǰ��Ҫ�ͷ�SDA��
uint8_t AHT21_I2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	AHT21_I2C_W_SDA(1);   //�л�������״̬,�����ͷ�SDA����ֹ�ӻ�����
	for (i = 0; i < 8; i ++)
	{
		AHT21_I2C_W_SCL(1);  //�ͷ�SCL��������������ȡ����λ
		if (AHT21_I2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		AHT21_I2C_W_SCL(0);//SCL��ȡ����λ�����ͣ�֮��ӻ��ͻὫ��һ������λ�ŵ�SDA��ѭ��8��
	}
	return Byte;
}

//����1  ��Ӧ��
//����0  Ӧ��
//����������Ӧ��	SCL�ͣ�������Ӧ��λ�ŵ�SDA(�ɸߵ���)������
//								SCL�ߣ��ӻ���SCL��ʱ��ȡӦ��λ������һλ
void AHT21_I2C_SendAck(uint8_t AckBit)
{
			//��������SCLĬ��Ϊ�͵�ƽ
	AHT21_I2C_W_SDA(AckBit);
	AHT21_I2C_W_SCL(1);  //�ͷ�SCL�󣬴ӻ���������ȡ����λ
	AHT21_I2C_W_SCL(0);
}

//����������Ӧ��	SCL�ͣ��ӻ���Ӧ��λ�ŵ�SDA(�ɸߵ���)������
//								SCL�ߣ�������SCL��ʱ��ȡӦ��λ������1λ����������֮ǰ��Ҫ�ͷ�SDA��
uint8_t AHT21_I2C_ReceiveAck(void)
{
			//��������SCLĬ��Ϊ�͵�ƽ
	uint8_t AckBit;
	AHT21_I2C_W_SDA(1);   //�л�SDAΪ����״̬,//�����ͷ�SDA����ֹ�ӻ�����
	AHT21_I2C_W_SCL(1);   //�ͷ�SCL��������������ȡ����λ
	AckBit = AHT21_I2C_R_SDA();
	AHT21_I2C_W_SCL(0);//SCL��ȡ����λ�����ͣ�֮��ӻ��ͻὫ��һ������λ�ŵ�Ӧ��λ��������һ��ʱ��Ԫ
	return AckBit;
}

void AHT21_Read_Continuous(u8 *pBuffer,u16 NumToRead)
{ 	    																 
		AHT21_I2C_Start();  
		AHT21_I2C_SendByte(0X71);   //����������ַ0X71,д���� 	 
		AHT21_I2C_ReceiveAck(); 
		while(NumToRead--)
		{
			*pBuffer++=AHT21_I2C_ReceiveByte();
			if(NumToRead==0)  //�������,�ظ�NACK
				AHT21_I2C_SendAck(1);
			else
				AHT21_I2C_SendAck(0);	//δ�������,�ظ�ACK
		}
		AHT21_I2C_Stop();//����һ��ֹͣ����	    
}  

void AHT21_Write_Continuous(u8 *pBuffer,u16 NumToWrite)
{
		AHT21_I2C_Start();  
		AHT21_I2C_SendByte(0X70);   //����������ַ0X70,д���� 
		AHT21_I2C_ReceiveAck();	   
		while(NumToWrite--)
		{		
			AHT21_I2C_SendByte(*pBuffer);     //�����ֽ�							   
			AHT21_I2C_ReceiveAck();
			pBuffer++;			
		}   	   
    AHT21_I2C_Stop();//����һ��ֹͣ���� 
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
			printf("У�鲻ͨ��\r\n");
		}
	}	
}
