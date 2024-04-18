#include "beep.h"

void beep_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //开启(使能)GPIOC时钟
	
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // 定义一个结构体变量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // BEEP-->PC0 端口配置
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // IO口速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 普通推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //根据设定参数初始化GPIOC0
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);//设置为低电平 关闭

}

//开启蜂鸣器
void beepon(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
}

//关闭蜂鸣器
void beepoff(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}
