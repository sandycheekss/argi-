#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "beep.h"
#include "usart.h"
#include "co2.h"
#include "iic1.h"
#include "usart3.h"
#include "usart2.h"
#include "lora.h"
#include "aht21.h"
#include "BH1750.h"
#include "bmp280.h"

uint8_t keyvalue;
//uint8_t a=10,b=11,c=12;
uint8_t sendbuff[5] = {0};
uint16_t light;
extern AHT21_DATE  AHT21_Date;
int main()
{
	NVIC_SetPriorityGrouping(2);//分组2
	SysTick_Config(72000);//设置系统定时器的中断周期为1ms
	Bmp280_Init();
	led_config();
	key_config();
	beep_config();
	usart_config();
//	I2C_config1();
	AHT21_Init();
//	BH1750_Init();
	usart2_config();
	lora_init();

//	u2_sendstr("123333\r\n",10);
	while(1)
	{
		if(ledtime[0] > ledtime[1])
		{
			ledturn(led1port,led1pin);
			printf("led已开启!\r\n");
			ledtime[0] = 0;
		}
		
		if(keytime[0] > keytime[1])
		{
			keyvalue = key_value();
			switch(keyvalue)
			{
				case 1:ledturn(led1port,led1pin);break;
				case 2:ledturn(led2port,led2pin);break;
			}
			keytime[0] = 0;
		}
		
		if(bmptime[0] > bmptime[1])
		{
			BMP280_GetData();
			bmptime[0] = 0;
		}
		
//		if(co2time[0] > co2time[1])
//		{
//			CO2_Read_Data();
//			co2time[0] = 0;
//		}
		
		if(ahttime[0] > ahttime[1])
		{
			AHT21_Handle();
			ahttime[0] = 0;
		}
		
//		if(lighttime[0] > lighttime[1])
//		{
//			light = LIght_Intensity();
//			printf("光照强度:%d\r\n",light);
//			lighttime[0] = 0;
//		}
		
		if(loratime[0] > loratime[1])
		{
			printf("已发送!\r\n");
			sendbuff[0] = 2;
			sendbuff[1] = AHT21_Date.Tem;
			sendbuff[2] = AHT21_Date.Hum;
			sendbuff[3] = BMP_Pressure/1000;
			sendbuff[4] = 2;
			Lora_Data(sendbuff, 5);
			loratime[0] = 0;
		}
		
	}
}
