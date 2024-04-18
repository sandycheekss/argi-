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
	//����GPIO    AUX M0 M1
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure = {0}; // ����һ���ṹ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_11;    //PA8 PA11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //ͨ���������
	GPIO_Init(GPIOA, &GPIO_InitStructure);      //��ʼ��PA8 PA11//M1
	//PA12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA12
//	GPIO_ResetBits(GPIOA, GPIO_Pin_12);//ģ���ϵ��AUX��������͵�ƽ//AUX PA6���Ǽ���

	usart2_config ();
	Lora_SetWordParam();
}

/*
	���� Lora ģ��Ĺ�������
	�����ÿ�ʼǰ��������ģʽ�ر��ж�
	������ɺ����һ��ģʽ���ж�
*/
void Lora_SetWordParam(void)
{
	//������lora�Ĺ��������Ĺ����в�Ӧ�ô����κ����ݣ������ٿ�ʼ����ǰ��Ҫ�رմ���3���ж�
	USART_ITConfig (USART2, USART_IT_RXNE, DISABLE);//�ر��ж�
	NVIC_DisableIRQ (USART2_IRQn); //ʧ���ж�ͨ����ȷ�������ù��������ڼ䲻�ᴥ�� USART3 �ж�
	//���ϵ���Ҫ��Lora����ģʽ����Ϊ����ģʽ
	Lora_SetMode(LORA_MODE_SLEEP);
	//���ù�������
	lora_parameter.lora_workprm.head = HEAD_POWERSAVE;//�����õĲ�������籣��
	lora_parameter.lora_workprm.addh = (LORA_SELF_ADDR >> 8) & 0xFF;//��ȡ��8λ
	lora_parameter.lora_workprm.addl = LORA_SELF_ADDR & 0xFF;//��ȡ��8λ
	lora_parameter.lora_workprm.sped = (SPED_UART_Parity_8N1<<6)|(SPED_UART_BPS_9600<<3)|(SPED_UART_SKYBPS_2_4);// ���ڲ���
	lora_parameter.lora_workprm.chan = 0x17;//Ŀ���ŵ�
	lora_parameter.lora_workprm.option = (OPTION_MODE_FP<<7)|(OPTION_IO_PP<<6)|(OPTION_WAKEUP_TIME_250<<3)|(OPTION_FEC_ON<<2)|(OPTION_TRANS_POWER_20);
	//��Lora�����������ò���
	lora_send(lora_parameter.buff,sizeof(lora_parameter.buff));//�����������壬����lora_parameter.buff�б������������õĹ�������
	delay_ms(100);//�ȴ�Lora�������
	//������ɣ�����һ��ģʽ
	Lora_SetMode(LORA_MODE_PATTERN);
	printf("�������\r\n");
	//���´��ж�
	USART_ITConfig (USART2, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ (USART2_IRQn);
}

/*
	����Lora��ģʽ
	�������� 0~4
	Lora_SetMode(0)Ϊһ��ģʽ
	Lora_SetMode(1)Ϊ����ģʽ
	Lora_SetMode(2)Ϊʡ��ģʽ
	Lora_SetMode(3)Ϊ����ģʽ
*/
void Lora_SetMode(u8 mode)
{
	//�ж�lora�Ĺ���ģʽ�Ƿ�Ϸ�������Ϊ����4��ģʽ�е�һ��
	if((mode != LORA_MODE_PATTERN) && (mode != LORA_MODE_WARKUP) && (mode != LORA_MODE_POWERSAVE) && (mode != LORA_MODE_SLEEP))  
		return ;
	GPIO_WriteBit(GPIOA,GPIO_Pin_8,(BitAction)(mode & 0x1));
	GPIO_WriteBit(GPIOA,GPIO_Pin_11,(BitAction)((mode>>4) & 0x1));
	delay_ms(100);
}

/*
    �� LoRa ģ�鷢���������������
	�� data �е�����ͨ�� USART3 ���͸� LoRa ģ�飬�������� LoRa ģ��Ĺ�������
*/
void lora_send (u8 *data, u8 size)
{
	for (u8 i = 0; i < size; i++)
	{
		u2_sendbit (data[i]);
	}
}

/*
	�� LoRa ģ�鷢������
    lora_target_addr = 0x0001
    0000 0000 0000 0001 >> 8  --->  0000 0000����8λ��& 1111 1111 --> 0000 0000  ---> 0x00
    0000 0000 0000 0001 & 1111 1111 ---> 0000 0001 ---> 0x01
*/
void Lora_Data (u8 *data, u16 len)
{
	u8 tar_addr1 = (LORA_TARGET_ADDR >> 8) & 0xFF; //��ȡĿ���ַ�ĸ�8λ����������Ϊ0x00����Ŀ���ַ��ǰ��λ
	u8 tar_addr2 = LORA_TARGET_ADDR & 0xFF;//��ȡĿ���ַ�ĵ�8λ����������Ϊ0x01����Ŀ���ַ�ĺ���λ
	uint8_t lora_buff[256] = {tar_addr1, tar_addr2, 0x17};//lora_buff�б�����Ŀ���ַ  Ŀ���ŵ�
	for(u8 i=0; i<len; i++)
	{
		lora_buff[i+3] = data[i];//����Ҫ���͵����ݱ��浽lora_buff��
	}
	for(u8 i=0; i < len+3; i++)
	{
		u2_sendbit(lora_buff[i]);//ͨ������2����
//		printf("data=%x\r\n",lora_buff[i]);
	}
}

/*
	Loraģ�鸴λ
*/
void Lora_Reset(void)
{
	u8 tx_buff[] = {0xC4,0xC4,0xC4};
	Lora_SetMode(LORA_MODE_SLEEP);//��������ģʽ
	lora_send(tx_buff,sizeof(tx_buff));//������������
}

void Lora_DataDeal(void)
{
	if(lora_receivecnt1 < 3)//˵�����յ�������֡��ʽ����
		return ;
	if(lora_receiveflag1 == 0)//˵����ʱ���ݻ�û�н�����ɣ�ֻ�е���1˵�����߿����ڽ�����һ֡���ݣ���һ֡�������
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
