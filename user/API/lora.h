#ifndef __LORA_H
#define __LORA_H

#include "stm32f10x.h"
extern uint32_t loratime[2];
#define LORA_TARGET_ADDR    	0x0001	//目标地址
#define LORA_SELF_ADDR			0x0003	//自身地址
#define LORA_MODE_PATTERN   	0x00	//一般模式
#define LORA_MODE_WARKUP    	0x01	//唤醒模式
#define LORA_MODE_POWERSAVE 	0x10	//省电模式
#define LORA_MODE_SLEEP     	0x11	//休眠模式

#define HEAD_POWERSAVE			0xC0	//所设置的参数会掉电保存
#define HEAD_POWERNOSAVE		0xC2	//所设置的参数不会掉电保存
//串口校验位
#define SPED_UART_Parity_8N1	0x00	//不奇偶校验
#define SPED_UART_Parity_801	0x01	//偶校验
#define SPED_UART_Parity_8E1	0x10	//奇校验
//串口波特率
#define SPED_UART_BPS_1200		0x00	
#define SPED_UART_BPS_2400		0x01
#define SPED_UART_BPS_4800		0x02
#define SPED_UART_BPS_9600		0x03	
#define SPED_UART_BPS_19200		0x04
#define SPED_UART_BPS_38400		0x05
#define SPED_UART_BPS_57600		0x06	
#define SPED_UART_BPS_115200	0x07
//串口空中速率
#define SPED_UART_SKYBPS_0_3	0x00	//0.3k
#define SPED_UART_SKYBPS_1_2	0x01	//1.2k	
#define SPED_UART_SKYBPS_2_4	0x02	//2.4k
#define SPED_UART_SKYBPS_4_8	0x03	//4.8k	
#define SPED_UART_SKYBPS_9_6	0x04	//9.6k
#define SPED_UART_SKYBPS_19_2	0x05	//19.2k
//传输模式
#define OPTION_MODE_TT			0x00	//透明传输模式
#define OPTION_MODE_FP			0x01	//定点传输模式
//IO 驱动方式
#define OPTION_IO_PP			0x01	//TXD、 AUX 推挽输出， RXD 上拉输入
#define OPTION_IO_OD			0x00	//TXD、 AUX 开路输出， RXD 开路输入
//无线唤醒时间(ms)
#define OPTION_WAKEUP_TIME_250	0x00
#define OPTION_WAKEUP_TIME_500	0x01
#define OPTION_WAKEUP_TIME_750	0x02
#define OPTION_WAKEUP_TIME_1000	0x03
#define OPTION_WAKEUP_TIME_1250	0x04
#define OPTION_WAKEUP_TIME_1500	0x05
#define OPTION_WAKEUP_TIME_1750	0x06
#define OPTION_WAKEUP_TIME_2000	0x07
//FEC 开关
#define OPTION_FEC_OFF			0x00	//关闭 FEC 纠错（ 默认）
#define OPTION_FEC_ON			0x01	//打开 FEC 纠错
//发射功率(大约值)(dBm)
#define OPTION_TRANS_POWER_20	0x00
#define OPTION_TRANS_POWER_17	0x01
#define OPTION_TRANS_POWER_14	0x10
#define OPTION_TRANS_POWER_10	0x11



void lora_init(void);
void usart3_init(uint32_t BaudRate);
void usart3_send(uint8_t data);
void lora_send(u8* data, u8 size);
void Lora_Data (uint8_t *data, u16 len);
void Lora_SetMode(u8 mode);
void Lora_SetWordParam(void);
void Lora_DataDeal(void);
void Lora_DataAnalysis(double data);


//参数设置结构体
typedef struct {
	u8 head;//表示参数的头部信息，通常用于标识参数的类型或特征
	u8 addh;//获取目标地址的高8位
    u8 addl;//获取目标地址的低8位
	u8 sped;//配置通信的速率，包括串口校验位、波特率和空中速率
	u8 chan;//目标信道
	u8 option;//包括各种通信选项，如传输模式、IO驱动方式、唤醒时间、FEC纠错开关以及发射功率。
}Lora_WorkParam;

//以两种不同的方式访问工作参数	
union LORA_WORKDATA{
	Lora_WorkParam lora_workprm;//通过结构体的方式访问工作参数
	u8 buff[6];//通过buff数组按照字节形式访问工作参数
};//接收或则发送工作参数


#endif
