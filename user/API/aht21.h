#ifndef __AHT21_H_
#define __AHT21_H_

#include "stm32f10x.h"

extern uint32_t ahttime[2];

typedef struct
{
	uint64_t Hum_Date;
	uint64_t Tem_Date;	
	float   Hum;
	float   Tem;
}AHT21_DATE;

extern uint8_t AHT21_State;   //0表示发送命令  1表示采集状态
extern uint8_t AHT21_Send_Cmd[3];
extern uint8_t AHT21_Receive_Date[7];
extern uint8_t AHT21_CRC;
extern AHT21_DATE  AHT21_Date;


void AHT21_Init(void);
void AHT21_I2C_Config(void);
void AHT21_I2C_W_SCL(uint8_t BitValue);
void AHT21_I2C_W_SDA(uint8_t BitValue);
uint8_t AHT21_I2C_R_SDA(void);
void AHT21_I2C_Start(void);
void AHT21_I2C_Stop(void);
void AHT21_I2C_SendByte(uint8_t Byte);
uint8_t AHT21_I2C_ReceiveByte(void);
void AHT21_I2C_SendAck(uint8_t AckBit);
uint8_t AHT21_I2C_ReceiveAck(void);


void AHT21_Init(void);
void AHT21_Read_Continuous(u8 *pBuffer,u16 NumToRead);
void AHT21_Write_Continuous(u8 *pBuffer,u16 NumToWrite);
unsigned char  CheckCrc8(unsigned char *pDat,unsigned char Lenth);
void AHT21_Handle(void);
#endif
