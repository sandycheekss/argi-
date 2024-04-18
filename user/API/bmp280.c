#include "bmp280.h"
#include "stdio.h"
#include "delay.h"

//初始化IIC接口
//函数:BMP280初始化
void Bmp280_Init(void)
{
	BMP280_I2C_Config();	//GPIO初始化
	Bmp_Init();	//矫正参数
}

//IIC引脚初始化  PB0 PB1 开漏输出
//PB0  SCL
//PB1  SDA
void BMP280_I2C_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);	
}

void BMP280_I2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_0, (BitAction)BitValue);
	delay_us(10);
}

void BMP280_I2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, (BitAction)BitValue);
	delay_us(10);
}

uint8_t BMP280_I2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
	delay_us(10);
	return BitValue;
}

//除了停止信号  其他的结束SCL都为低电平
//函数：起始条件	当SCL为高电平，SDA由高到低
void BMP280_I2C_Start(void)
{
	BMP280_I2C_W_SDA(1);   //保险起见先释放SDA,如果原来是高也不影响
	BMP280_I2C_W_SCL(1);
	BMP280_I2C_W_SDA(0);
	BMP280_I2C_W_SCL(0);
}

//函数：终止条件	当SCL为高电平，SDA由低到高
void BMP280_I2C_Stop(void)
{
	BMP280_I2C_W_SDA(0);
	BMP280_I2C_W_SCL(1);
	BMP280_I2C_W_SDA(1);
}

//函数：发送一个字节	SCL低，主机将数据位放到SDA(由高到低)，拉高
//								SCL高，从机在SCL高时读取数据位，循环八次，发送一个字节
void BMP280_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		BMP280_I2C_W_SDA(Byte & (0x80 >> i));
		BMP280_I2C_W_SCL(1);   //释放SCL后，从机会立即读取数据位
		BMP280_I2C_W_SCL(0);
	}
}

//函数：接收一个字节	SCL低，从机将数据位放到SDA(由高到低)，拉高
//								SCL高，主机在SCL高时读取数据位，循环八次，接收一个字节（主机接收之前需要释放SDA）
uint8_t BMP280_I2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	BMP280_I2C_W_SDA(1);   //切换成输入状态,主机释放SDA，防止从机干扰
	for (i = 0; i < 8; i ++)
	{
		BMP280_I2C_W_SCL(1);  //释放SCL后，主机会立即读取数据位
		if (BMP280_I2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		BMP280_I2C_W_SCL(0);//SCL读取数据位后，拉低，之后从机就会将下一个数据位放到SDA，循环8次
	}
	return Byte;
}

//数据1  非应答
//数据0  应答
//函数：发送应答	SCL低，主机将应答位放到SDA(由高到低)，拉高
//								SCL高，从机在SCL高时读取应答位，发送一位
void BMP280_I2C_SendAck(uint8_t AckBit)
{
			//函数进来SCL默认为低电平
	BMP280_I2C_W_SDA(AckBit);
	BMP280_I2C_W_SCL(1);  //释放SCL后，从机会立即读取数据位
	BMP280_I2C_W_SCL(0);
}

//函数：接收应答	SCL低，从机将应答位放到SDA(由高到低)，拉高
//								SCL高，主机在SCL高时读取应答位，接收1位（主机接收之前需要释放SDA）
uint8_t BMP280_I2C_ReceiveAck(void)
{
			//函数进来SCL默认为低电平
	uint8_t AckBit;
	BMP280_I2C_W_SDA(1);   //切换SDA为输入状态,//主机释放SDA，防止从机干扰
	BMP280_I2C_W_SCL(1);   //释放SCL后，主机会立即读取数据位
	AckBit = BMP280_I2C_R_SDA();
	BMP280_I2C_W_SCL(0);//SCL读取数据位后，拉低，之后从机就会将下一个数据位放到应答位，进入下一个时序单元
	return AckBit;
}

//函数:BMP280向寄存器写数据
/*Parameter Configuration*/
double	BMP_Pressure,BMP_Temperature;

void BMP280_WriteReg(uint8_t RegAddress, uint8_t Data)
{
		BMP280_I2C_Start();
		BMP280_I2C_SendByte(BMP280_ADDRESS);	//寻址找到从机+写信号
		BMP280_I2C_ReceiveAck();
		BMP280_I2C_SendByte(RegAddress);
		BMP280_I2C_ReceiveAck();
		BMP280_I2C_SendByte(Data);
		BMP280_I2C_ReceiveAck();
		BMP280_I2C_Stop();
}

//函数:BMP280向寄存器读数据
uint8_t BMP280_ReadReg(uint8_t RegAddress)
{
		uint8_t Data;
		BMP280_I2C_Start();
		BMP280_I2C_SendByte(BMP280_ADDRESS);	//寻址找到从机+写信号	1110 110 0
		BMP280_I2C_ReceiveAck();
	
		BMP280_I2C_SendByte(RegAddress);
		BMP280_I2C_ReceiveAck();
		
		//转入读时序(1)，重新指定读写位，必须重新起始
		BMP280_I2C_Start();
		BMP280_I2C_SendByte(BMP280_ADDRESS | 0x01);	//寻址找到从机+读信号	1110 110 0
		BMP280_I2C_ReceiveAck();
	
		Data = BMP280_I2C_ReceiveByte();
		BMP280_I2C_SendAck(1);//读一个字节置1结束，主机收回总线控制权
		BMP280_I2C_Stop();
		
		return Data;
}


uint8_t BMP280_ReadID(void)
{
	uint8_t ID;;
	BMP280_I2C_SendByte(BMP280_CHIPID_REG);
	ID = BMP280_I2C_ReceiveByte();
	return ID;
}

//函数：矫正参数
BMP280 bmp280_inst;
BMP280* bmp280 = &bmp280_inst;		//这个全局结构体变量用来保存存在芯片内ROM补偿参数
void Bmp_Init(void)
{
	u8 Lsb,Msb;
	
	/********************接下来读出矫正参数*********************/
	//温度传感器的矫正值
	Lsb = BMP280_ReadReg(BMP280_DIG_T1_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_T1_MSB_REG);
	bmp280->T1 = (((u16)Msb)<<8) + Lsb;			//高位加低位
	Lsb = BMP280_ReadReg(BMP280_DIG_T2_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_T2_MSB_REG);
	bmp280->T2 = (((u16)Msb)<<8) + Lsb;		
	Lsb = BMP280_ReadReg(BMP280_DIG_T3_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_T3_MSB_REG);
	bmp280->T3 = (((u16)Msb)<<8) + Lsb;		
	
	//大气压传感器的矫正值
	Lsb = BMP280_ReadReg(BMP280_DIG_P1_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P1_MSB_REG);
	bmp280->P1 = (((u16)Msb)<<8) + Lsb;		
	Lsb = BMP280_ReadReg(BMP280_DIG_P2_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P2_MSB_REG);
	bmp280->P2 = (((u16)Msb)<<8) + Lsb;	
	Lsb = BMP280_ReadReg(BMP280_DIG_P3_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P3_MSB_REG);
	bmp280->P3 = (((u16)Msb)<<8) + Lsb;	
	Lsb = BMP280_ReadReg(BMP280_DIG_P4_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P4_MSB_REG);
	bmp280->P4 = (((u16)Msb)<<8) + Lsb;	
	Lsb = BMP280_ReadReg(BMP280_DIG_P5_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P5_MSB_REG);
	bmp280->P5 = (((u16)Msb)<<8) + Lsb;	
	Lsb = BMP280_ReadReg(BMP280_DIG_P6_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P6_MSB_REG);
	bmp280->P6 = (((u16)Msb)<<8) + Lsb;	
	Lsb = BMP280_ReadReg(BMP280_DIG_P7_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P7_MSB_REG);
	bmp280->P7 = (((u16)Msb)<<8) + Lsb;	
	Lsb = BMP280_ReadReg(BMP280_DIG_P8_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P8_MSB_REG);
	bmp280->P8 = (((u16)Msb)<<8) + Lsb;	
	Lsb = BMP280_ReadReg(BMP280_DIG_P9_LSB_REG);
	Msb = BMP280_ReadReg(BMP280_DIG_P9_MSB_REG);
	bmp280->P9 = (((u16)Msb)<<8) + Lsb;	
	/******************************************************/
	BMP280_WriteReg(BMP280_RESET_REG,BMP280_RESET_VALUE);	//往复位寄存器写入给定值
	
	BMP_OVERSAMPLE_MODE			BMP_OVERSAMPLE_MODEStructure;
	BMP_OVERSAMPLE_MODEStructure.P_Osample = BMP280_P_MODE_3;
	BMP_OVERSAMPLE_MODEStructure.T_Osample = BMP280_T_MODE_1;
	BMP_OVERSAMPLE_MODEStructure.WORKMODE  = BMP280_NORMAL_MODE;
	BMP280_Set_TemOversamp(&BMP_OVERSAMPLE_MODEStructure);
	
	BMP_CONFIG					BMP_CONFIGStructure;
	BMP_CONFIGStructure.T_SB = BMP280_T_SB1;
	BMP_CONFIGStructure.FILTER_COEFFICIENT = BMP280_FILTER_MODE_4;
	BMP_CONFIGStructure.SPI_EN = DISABLE;
	
	BMP280_Set_Standby_FILTER(&BMP_CONFIGStructure);
}

//设置BMP过采样因子 MODE 
//BMP280_SLEEP_MODE||BMP280_FORCED_MODE||BMP280_NORMAL_MODE
void BMP280_Set_TemOversamp(BMP_OVERSAMPLE_MODE * Oversample_Mode)
{
	u8 Regtmp;
	Regtmp = ((Oversample_Mode->T_Osample)<<5)|
			 ((Oversample_Mode->P_Osample)<<2)|
			 ((Oversample_Mode)->WORKMODE);
	
	BMP280_WriteReg(BMP280_CTRLMEAS_REG,Regtmp);
}


//设置保持时间和滤波器分频因子
void BMP280_Set_Standby_FILTER(BMP_CONFIG * BMP_Config)
{
	u8 Regtmp;
	Regtmp = ((BMP_Config->T_SB)<<5)|
			 ((BMP_Config->FILTER_COEFFICIENT)<<2)|
			 ((BMP_Config->SPI_EN));
	
	BMP280_WriteReg(BMP280_CONFIG_REG,Regtmp);
}

//获取BMP当前状态
//status_flag = BMP280_MEASURING ||
//			 	BMP280_IM_UPDATE
u8  BMP280_GetStatus(u8 status_flag)
{
	u8 flag;
	flag = BMP280_ReadReg(BMP280_STATUS_REG);
	if(flag&status_flag)	return SET;
	else return RESET;
}




/*******************主要部分*********************/
/****************获取传感器精确值****************/
//大气压值-Pa
double BMP280_Get_Pressure(void)
{
	
	uint8_t XLsb,Lsb, Msb;
	long signed Bit32;
	double pressure;
	XLsb = BMP280_ReadReg(BMP280_PRESSURE_XLSB_REG);
	Lsb	 = BMP280_ReadReg(BMP280_PRESSURE_LSB_REG);
	Msb	 = BMP280_ReadReg(BMP280_PRESSURE_MSB_REG);
	Bit32 = ((long)(Msb << 12))|((long)(Lsb << 4))|(XLsb>>4);	//寄存器的值,组成一个浮点数
	pressure = bmp280_compensate_P_double(Bit32);
	return pressure;
}

//温度值-℃
double BMP280_Get_Temperature(void)
{
	uint8_t XLsb,Lsb, Msb;
	long signed Bit32;
	double temperature;
	XLsb = BMP280_ReadReg(BMP280_TEMPERATURE_XLSB_REG);
	Lsb	 = BMP280_ReadReg(BMP280_TEMPERATURE_LSB_REG);
	Msb	 = BMP280_ReadReg(BMP280_TEMPERATURE_MSB_REG);
	Bit32 = ((long)(Msb << 12))|((long)(Lsb << 4))|(XLsb>>4);	//寄存器的值,组成一个浮点数
	temperature = bmp280_compensate_T_double(Bit32);
	return temperature; // 返回值
}

//函数：获取温度，大气压数据
void BMP280_GetData(void)
{
	
		while(BMP280_GetStatus(BMP280_MEASURING) != RESET);
		while(BMP280_GetStatus(BMP280_IM_UPDATE) != RESET);
//		BMP_Temperature = BMP280_Get_Temperature();
		BMP_Pressure = BMP280_Get_Pressure();
//		printf("温度Temperature:%.2f℃\r\n",BMP_Temperature);
		printf("气压Pressure:%.2fKPa\r\n",BMP_Pressure/1000);
}

/***************************************END OF LINE*********************************************/

/**************************传感器值转定点值*************************************/
BMP280_S32_t t_fine;			//用于计算补偿
//我用浮点补偿
#ifdef USE_FIXED_POINT_COMPENSATE
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. 
// t_fine carries fine temperature as global value
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
	BMP280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) * 
	((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
	BMP280_S64_t var1, var2, p;
	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
	var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
	return (BMP280_U32_t)p;
}


/***********************************CUT*************************************/
#else
/**************************传感器值转定点值*************************************/
// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
	double var1, var2, T;
	var1 = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
	var2 = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0) *
	(((double)adc_T)/131072.0 - ((double) dig_T1)/8192.0)) * ((double)dig_T3);
	t_fine = (BMP280_S32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
	double var1, var2, p;
	var1 = ((double)t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
	var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);
	if (var1 == 0.0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double)adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
	return p;
}

#endif
