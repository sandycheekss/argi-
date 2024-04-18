#ifndef _BMP280_H_
#define _BMP280_H_

#include "stm32f10x.h"
extern uint32_t bmptime[2];
extern double	BMP_Pressure;
#define BMP280_ADDRESS						  				 0xEC		//���豸��ַ
#define BMP280_RESET_VALUE			 		  			 0xB6		//��λ�Ĵ���д��ֵ

#define BMP280_CHIPID_REG                    0xD0  /*Chip ID Register */
#define BMP280_RESET_REG                     0xE0  /*Softreset Register */
#define BMP280_STATUS_REG                    0xF3  /*Status Register */
#define BMP280_CTRLMEAS_REG                  0xF4  /*Ctrl Measure Register */
#define BMP280_CONFIG_REG                    0xF5  /*Configuration Register */
#define BMP280_PRESSURE_MSB_REG              0xF7  /*Pressure MSB Register */
#define BMP280_PRESSURE_LSB_REG              0xF8  /*Pressure LSB Register */
#define BMP280_PRESSURE_XLSB_REG             0xF9  /*Pressure XLSB Register */
#define BMP280_TEMPERATURE_MSB_REG           0xFA  /*Temperature MSB Reg */
#define BMP280_TEMPERATURE_LSB_REG           0xFB  /*Temperature LSB Reg */
#define BMP280_TEMPERATURE_XLSB_REG          0xFC  /*Temperature XLSB Reg */
//״̬�Ĵ���ת����־
#define	BMP280_MEASURING					0x01
#define	BMP280_IM_UPDATE					0x08

/*calibration parameters */
#define BMP280_DIG_T1_LSB_REG                0x88
#define BMP280_DIG_T1_MSB_REG                0x89
#define BMP280_DIG_T2_LSB_REG                0x8A
#define BMP280_DIG_T2_MSB_REG                0x8B
#define BMP280_DIG_T3_LSB_REG                0x8C
#define BMP280_DIG_T3_MSB_REG                0x8D
#define BMP280_DIG_P1_LSB_REG                0x8E
#define BMP280_DIG_P1_MSB_REG                0x8F
#define BMP280_DIG_P2_LSB_REG                0x90
#define BMP280_DIG_P2_MSB_REG                0x91
#define BMP280_DIG_P3_LSB_REG                0x92
#define BMP280_DIG_P3_MSB_REG                0x93
#define BMP280_DIG_P4_LSB_REG                0x94
#define BMP280_DIG_P4_MSB_REG                0x95
#define BMP280_DIG_P5_LSB_REG                0x96
#define BMP280_DIG_P5_MSB_REG                0x97
#define BMP280_DIG_P6_LSB_REG                0x98
#define BMP280_DIG_P6_MSB_REG                0x99
#define BMP280_DIG_P7_LSB_REG                0x9A
#define BMP280_DIG_P7_MSB_REG                0x9B
#define BMP280_DIG_P8_LSB_REG                0x9C
#define BMP280_DIG_P8_MSB_REG                0x9D
#define BMP280_DIG_P9_LSB_REG                0x9E
#define BMP280_DIG_P9_MSB_REG                0x9F


/*******************************�������������㲹��ֵ���**********************************/
typedef			long signed int				BMP280_S32_t;	//�з��� 64λ��
typedef			long unsigned int			BMP280_U32_t;	//�޷��� 32λ��
typedef			long long signed int		BMP280_S64_t;

#define	dig_T1			bmp280->T1	
#define	dig_T2			bmp280->T2	
#define	dig_T3			bmp280->T3	

#define	dig_P1			bmp280->P1
#define	dig_P2			bmp280->P2
#define	dig_P3			bmp280->P3
#define	dig_P4			bmp280->P4
#define	dig_P5			bmp280->P5
#define	dig_P6			bmp280->P6
#define	dig_P7			bmp280->P7
#define	dig_P8			bmp280->P8
#define	dig_P9			bmp280->P9
/************************************************CUT****************************************/


//BMP����ģʽ
typedef enum {
	BMP280_SLEEP_MODE = 0x0,
	BMP280_FORCED_MODE = 0x1,	//����˵0x2
	BMP280_NORMAL_MODE = 0x3
} BMP280_WORK_MODE;

//BMPѹ������������
typedef enum 
{
	BMP280_P_MODE_SKIP = 0x0,	/*skipped*/
	BMP280_P_MODE_1,			/*x1*/
	BMP280_P_MODE_2,			/*x2*/
	BMP280_P_MODE_3,			/*x4*/
	BMP280_P_MODE_4,			/*x8*/
	BMP280_P_MODE_5			    /*x16*/
} BMP280_P_OVERSAMPLING;	

//BMP�¶ȹ���������
typedef enum {
	BMP280_T_MODE_SKIP = 0x0,	/*skipped*/
	BMP280_T_MODE_1,			/*x1*/
	BMP280_T_MODE_2,			/*x2*/
	BMP280_T_MODE_3,			/*x4*/
	BMP280_T_MODE_4,			/*x8*/
	BMP280_T_MODE_5			    /*x16*/
} BMP280_T_OVERSAMPLING;
									
//IIR�˲���ʱ�䳣��
typedef enum {
	BMP280_FILTER_OFF = 0x0,	/*filter off*/
	BMP280_FILTER_MODE_1,		/*0.223*ODR*/	/*x2*/
	BMP280_FILTER_MODE_2,		/*0.092*ODR*/	/*x4*/
	BMP280_FILTER_MODE_3,		/*0.042*ODR*/	/*x8*/
	BMP280_FILTER_MODE_4		/*0.021*ODR*/	/*x16*/
} BMP280_FILTER_COEFFICIENT;

//����ʱ��
typedef enum {
	BMP280_T_SB1 = 0x0,	    /*0.5ms*/
	BMP280_T_SB2,			/*62.5ms*/
	BMP280_T_SB3,			/*125ms*/
	BMP280_T_SB4,			/*250ms*/
	BMP280_T_SB5,			/*500ms*/
	BMP280_T_SB6,			/*1000ms*/
	BMP280_T_SB7,			/*2000ms*/
	BMP280_T_SB8,			/*4000ms*/
} BMP280_T_SB;


typedef struct  
{
	/* T1~P9 Ϊ����ϵ�� */
	uint16_t T1;
	int16_t	T2;
	int16_t	T3;
	uint16_t P1;
	int16_t	P2;
	int16_t	P3;
	int16_t	P4;
	int16_t	P5;
	int16_t	P6;
	int16_t	P7;
	int16_t	P8;
	int16_t	P9;
} BMP280;


typedef struct
{
	BMP280_P_OVERSAMPLING P_Osample;
	BMP280_T_OVERSAMPLING T_Osample;
	BMP280_WORK_MODE		WORKMODE;
} BMP_OVERSAMPLE_MODE;
	
typedef struct
{
	BMP280_T_SB 				T_SB;
	BMP280_FILTER_COEFFICIENT 	FILTER_COEFFICIENT;
	FunctionalState				SPI_EN;
} BMP_CONFIG;

extern BMP280* bmp280;


void Bmp280_Init(void);
void BMP280_I2C_Config(void);
void BMP280_I2C_W_SCL(uint8_t BitValue);
void BMP280_I2C_W_SDA(uint8_t BitValue);
uint8_t BMP280_I2C_R_SDA(void);
void BMP280_I2C_Start(void);
void BMP280_I2C_Stop(void);
void BMP280_I2C_SendByte(uint8_t Byte);
uint8_t BMP280_I2C_ReceiveByte(void);
void BMP280_I2C_SendAck(uint8_t AckBit);
uint8_t BMP280_I2C_ReceiveAck(void);




void BMP280_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t BMP280_ReadReg(uint8_t RegAddress);
void Bmp280_Init(void);
uint8_t BMP280_ReadID(void);

void Bmp_Init(void);
void BMP280_Set_TemOversamp(BMP_OVERSAMPLE_MODE * Oversample_Mode);
void BMP280_Set_Standby_FILTER(BMP_CONFIG * BMP_Config);
u8  BMP280_GetStatus(u8 status_flag);
double BMP280_Get_Pressure(void);
double BMP280_Get_Temperature(void);
void BMP280_GetData(void);

double bmp280_compensate_T_double(BMP280_S32_t adc_T);
double bmp280_compensate_P_double(BMP280_S32_t adc_P);

#endif