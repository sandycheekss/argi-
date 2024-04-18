#ifndef __IIC1_H
#define __IIC1_H

#include "stm32f10x.h"
#define IIC1_SCL_OUT1  GPIO_SetBits(GPIOC, GPIO_Pin_3)
#define IIC1_SCL_OUT0  GPIO_ResetBits(GPIOC, GPIO_Pin_3)
//#define IIC1_SCL_IN    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)

#define IIC1_SDA_OUT1  GPIO_SetBits(GPIOC, GPIO_Pin_13)
#define IIC1_SDA_OUT0  GPIO_ResetBits(GPIOC, GPIO_Pin_13)
#define IIC1_SDA_IN    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)

void I2C_config1(void);
void IIC1_Start(void);
void IIC1_Stop(void);
void IIC1_Send_Ack(uint8_t ack);
uint8_t IIC1_Revice_Ack(void);
uint8_t IIC1_Send_Byte(uint8_t data);
uint8_t IIC1_Revice_Byte(uint8_t ack);

uint8_t Calc_CRC8(uint8_t *dat, uint8_t Num);

#endif
