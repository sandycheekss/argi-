#include "iic1.h"
#include "delay.h"
#include "stdio.h"
//I2C初始化
//I2C通信 PC3 -- SCL   PC13 -- SDA
void I2C_config1(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0}; // 定义一个结构体变量
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_Out_OD; //开漏输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);      //初始化PB
//	GPIO_SetBits(GPIOC,GPIO_Pin_3|GPIO_Pin_13);
	
	//总线在空闲状态
	IIC1_SCL_OUT1;  //1为高 0为低
	IIC1_SDA_OUT1;
}

//开始信号
void IIC1_Start(void)
{
	//硬件问题 开始SCL为低
	IIC1_SCL_OUT0;
	delay_us(6);
	//1.SCL SDA 置高
	IIC1_SDA_OUT1;
	IIC1_SCL_OUT1;
	//2.延时完 SCL SDA置低
	delay_us(6);//延时---起始条件的建立时间
	IIC1_SDA_OUT0;//---产生起始条件
	delay_us(6);//延时---起始条件的保持时间
	IIC1_SCL_OUT0; //---结束起始条件  准备接收/发送数据
}

//结束信号
void IIC1_Stop(void)
{
	//1.先拉低SDA，再拉低SCL
	IIC1_SDA_OUT0;
	IIC1_SCL_OUT0;
	//2.拉高SCL
	IIC1_SCL_OUT1;
	delay_us(6);//延时---停止条件的建立时间
	//3.拉高SDA
	IIC1_SDA_OUT1; //---产生了停止条件
	delay_us(6);//延时---本次通信结束到下次通信开始的时间

}

//主机发送应答信号
void IIC1_Send_Ack(uint8_t ack)
{
	IIC1_SCL_OUT0;
	if(ack)//(主机准备数据)
		IIC1_SDA_OUT1; //无应答
	else
		IIC1_SDA_OUT0; //有应答
	delay_us(6);//延时（数据稳定在数据线上）
	IIC1_SCL_OUT1;//（从机在时钟线上升沿从SDA上采集数据）
	delay_us(6);//延时（给时间从机读取数据）
	
	IIC1_SCL_OUT0;//方便后续的操作；防止意外产生了停止条件
}

//主机读取应答信号    0--有应答  1--无应答
uint8_t IIC1_Revice_Ack(void)
{
	uint8_t ack=0;
	//1.从机准备数据
	IIC1_SCL_OUT0;//（从机准备数据）
	//2.SDA SCL 拉高
	IIC1_SDA_OUT1;//读模式-----让输出电路与管脚断开！！！！！！！！！！
	delay_us(6);//延时（给时间从机准备数据并且数据稳定在数据线上）
	IIC1_SCL_OUT1;
	delay_us(6);//延时 (给时间主机读取数据)
	//3.读取SDA数据 1--高电平--无应答  0 --低电平 -- 应答
	if(IIC1_SDA_IN)//主机读取SDA线上的数据
		ack=1;
	
	//IIC1_SCL=0;//方便后续的操作；防止意外产生了停止条件
	return ack;
}

//主机发送一个字节给从机并且读取一次应答信号  应答信号 0--有应答  1--非应答
uint8_t IIC1_Send_Byte(uint8_t data)
{
	uint8_t i=0;

	for(i=0;i<8;i++)
	{
		IIC1_SCL_OUT0;
		if((data<<i)&0x80)//(主机准备数据)  
			IIC1_SDA_OUT1;           
		else
			IIC1_SDA_OUT0;
		delay_us(6);//延时（数据稳定在数据线上）
		IIC1_SCL_OUT1;//（从机在时钟线上升沿从SDA上采集数据）
		delay_us(6);//延时（给时间从机读取数据）
	}
	//IIC1_SCL=0;//方便后续的操作；防止意外产生了停止条件
	
	return IIC1_Revice_Ack();
}

//主机读取一个字节并且发送一次应答信号   应答信号 0--有应答  1--非应答
uint8_t IIC1_Revice_Byte(uint8_t ack)
{
	uint8_t i=0;
	uint8_t data=0;
	
	for(i=0;i<8;i++)
	{
		IIC1_SCL_OUT0;//（从机准备数据）
		IIC1_SDA_OUT1;//读模式-----让输出电路与管脚断开！！！！！！！！！！
		delay_us(6);//延时（给时间从机准备数据并且数据稳定在数据线上）
		IIC1_SCL_OUT1;
		data = (data<<1) | (IIC1_SDA_IN);
		
		delay_us(6);//延时 (给时间主机读取数据)
		
	}
	//IIC1_SCL=0;//方便后续的操作；防止意外产生了停止条件
	delay_us(6);
	IIC1_Send_Ack(ack);
	delay_us(6);
	return data;
}

//校验方式
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
