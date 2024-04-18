#include "lora.h"
#include "usart2.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"

union LORA_WORKDATA lora_parameter = {0};
uint8_t lora_data[64];

extern uint8_t lora_receivecnt1;
extern uint8_t lora_receiveflag1;
extern uint8_t lora_receivebuf1[32];


void lora_init (void)
{
	//配置GPIO    AUX M0 M1
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // 定义一个结构体变量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_11;    //PA8 PA11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //通用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);      //初始化PA8 PA11//M1
	//PA12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA12
//	GPIO_ResetBits(GPIOA, GPIO_Pin_12);//模块上电后，AUX立即输出低电平//AUX PA6这是几？

	usart2_config ();
	Lora_SetWordParam();
}

/*
	设置 Lora 模块的工作参数
	在设置开始前进入休眠模式关闭中断
	配置完成后进入一般模式打开中断
*/
void Lora_SetWordParam(void)
{
	//在配置lora的工作参数的过程中不应该传输任何数据，所以再开始配置前需要关闭串口3的中断
	USART_ITConfig (USART2, USART_IT_RXNE, DISABLE);//关闭中断
	NVIC_DisableIRQ (USART2_IRQn); //失能中断通道，确保在设置工作参数期间不会触发 USART3 中断
	//刚上电需要将Lora工作模式设置为休眠模式
	Lora_SetMode(LORA_MODE_SLEEP);
	//设置工作参数
	lora_parameter.lora_workprm.head = HEAD_POWERSAVE;//所设置的参数会掉电保存
	lora_parameter.lora_workprm.addh = (LORA_SELF_ADDR >> 8) & 0xFF;//获取高8位
	lora_parameter.lora_workprm.addl = LORA_SELF_ADDR & 0xFF;//获取低8位
	lora_parameter.lora_workprm.sped = (SPED_UART_Parity_8N1<<6)|(SPED_UART_BPS_9600<<3)|(SPED_UART_SKYBPS_2_4);// 串口参数
	lora_parameter.lora_workprm.chan = 0x17;//目标信道
	lora_parameter.lora_workprm.option = (OPTION_MODE_FP<<7)|(OPTION_IO_PP<<6)|(OPTION_WAKEUP_TIME_250<<3)|(OPTION_FEC_ON<<2)|(OPTION_TRANS_POWER_20);
	//向Lora发送命令配置参数
	lora_send(lora_parameter.buff,sizeof(lora_parameter.buff));//由于是联合体，所以lora_parameter.buff中保存了上面配置的工作参数
	delay_ms(100);//等待Lora完成配置
	//配置完成，进入一般模式
	Lora_SetMode(LORA_MODE_PATTERN);
	printf("配置完成\r\n");
	//重新打开中断
	USART_ITConfig (USART2, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ (USART2_IRQn);
}

/*
	设置Lora的模式
	参数可填 0~4
	Lora_SetMode(0)为一般模式
	Lora_SetMode(1)为唤醒模式
	Lora_SetMode(2)为省点模式
	Lora_SetMode(3)为休眠模式
*/
void Lora_SetMode(u8 mode)
{
	//判断lora的工作模式是否合法，必须为以下4中模式中的一种
	if((mode != LORA_MODE_PATTERN) && (mode != LORA_MODE_WARKUP) && (mode != LORA_MODE_POWERSAVE) && (mode != LORA_MODE_SLEEP))  
		return ;
	GPIO_WriteBit(GPIOA,GPIO_Pin_8,(BitAction)(mode & 0x1));
	GPIO_WriteBit(GPIOA,GPIO_Pin_11,(BitAction)((mode>>4) & 0x1));
	delay_ms(100);
}

/*
    向 LoRa 模块发送命令或配置数据
	将 data 中的数据通过 USART3 发送给 LoRa 模块，用于配置 LoRa 模块的工作参数
*/
void lora_send (u8 *data, u8 size)
{
	for (u8 i = 0; i < size; i++)
	{
		u2_sendbit (data[i]);
	}
}

/*
	向 LoRa 模块发送数据
    lora_target_addr = 0x0001
    0000 0000 0000 0001 >> 8  --->  0000 0000（高8位）& 1111 1111 --> 0000 0000  ---> 0x00
    0000 0000 0000 0001 & 1111 1111 ---> 0000 0001 ---> 0x01
*/
void Lora_Data (u8 *data, u16 len)
{
	u8 tar_addr1 = (LORA_TARGET_ADDR >> 8) & 0xFF; //获取目标地址的高8位，这里计算后为0x00，即目标地址的前两位
	u8 tar_addr2 = LORA_TARGET_ADDR & 0xFF;//获取目标地址的低8位，这里计算后为0x01，即目标地址的后两位
	uint8_t lora_buff[256] = {tar_addr1, tar_addr2, 0x17};//lora_buff中保存了目标地址  目标信道
	for(u8 i=0; i<len; i++)
	{
		lora_buff[i+3] = data[i];//将需要发送的数据保存到lora_buff中
	}
	for(u8 i=0; i < len+3; i++)
	{
		u2_sendbit(lora_buff[i]);//通过串口2发送
//		printf("data=%x\r\n",lora_buff[i]);
	}
}

/*
	Lora模块复位
*/
void Lora_Reset(void)
{
	u8 tx_buff[] = {0xC4,0xC4,0xC4};
	Lora_SetMode(LORA_MODE_SLEEP);//进入休眠模式
	lora_send(tx_buff,sizeof(tx_buff));//发送重启命令
}

void Lora_DataDeal(void)
{
	if(lora_receivecnt1 < 3)//说明接收到的数据帧格式错误
		return ;
	if(lora_receiveflag1 == 0)//说明此时数据还没有接收完成，只有等于1说明总线空闲在接受下一帧数据，上一帧接收完成
		return ;
	for(u8 i=0;i<lora_receivecnt1;i++)
	{
		lora_data[i] = lora_receivebuf1[i];
		printf("%02X\t",lora_data[i]);
//		printf("%02X\t",uart3_recv.Recv_Data[i]);
	}
	printf("\r\n");
	lora_receivecnt1 = 0;
	lora_receiveflag1 = 0;
	memset(lora_receivebuf1,0,sizeof(lora_receivebuf1));
}
