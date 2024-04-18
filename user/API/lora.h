#ifndef __LORA_H
#define __LORA_H

#include "stm32f10x.h"
extern uint32_t loratime[2];
#define LORA_TARGET_ADDR    	0x0001	//Ŀ���ַ
#define LORA_SELF_ADDR			0x0003	//�����ַ
#define LORA_MODE_PATTERN   	0x00	//һ��ģʽ
#define LORA_MODE_WARKUP    	0x01	//����ģʽ
#define LORA_MODE_POWERSAVE 	0x10	//ʡ��ģʽ
#define LORA_MODE_SLEEP     	0x11	//����ģʽ

#define HEAD_POWERSAVE			0xC0	//�����õĲ�������籣��
#define HEAD_POWERNOSAVE		0xC2	//�����õĲ���������籣��
//����У��λ
#define SPED_UART_Parity_8N1	0x00	//����żУ��
#define SPED_UART_Parity_801	0x01	//żУ��
#define SPED_UART_Parity_8E1	0x10	//��У��
//���ڲ�����
#define SPED_UART_BPS_1200		0x00	
#define SPED_UART_BPS_2400		0x01
#define SPED_UART_BPS_4800		0x02
#define SPED_UART_BPS_9600		0x03	
#define SPED_UART_BPS_19200		0x04
#define SPED_UART_BPS_38400		0x05
#define SPED_UART_BPS_57600		0x06	
#define SPED_UART_BPS_115200	0x07
//���ڿ�������
#define SPED_UART_SKYBPS_0_3	0x00	//0.3k
#define SPED_UART_SKYBPS_1_2	0x01	//1.2k	
#define SPED_UART_SKYBPS_2_4	0x02	//2.4k
#define SPED_UART_SKYBPS_4_8	0x03	//4.8k	
#define SPED_UART_SKYBPS_9_6	0x04	//9.6k
#define SPED_UART_SKYBPS_19_2	0x05	//19.2k
//����ģʽ
#define OPTION_MODE_TT			0x00	//͸������ģʽ
#define OPTION_MODE_FP			0x01	//���㴫��ģʽ
//IO ������ʽ
#define OPTION_IO_PP			0x01	//TXD�� AUX ��������� RXD ��������
#define OPTION_IO_OD			0x00	//TXD�� AUX ��·����� RXD ��·����
//���߻���ʱ��(ms)
#define OPTION_WAKEUP_TIME_250	0x00
#define OPTION_WAKEUP_TIME_500	0x01
#define OPTION_WAKEUP_TIME_750	0x02
#define OPTION_WAKEUP_TIME_1000	0x03
#define OPTION_WAKEUP_TIME_1250	0x04
#define OPTION_WAKEUP_TIME_1500	0x05
#define OPTION_WAKEUP_TIME_1750	0x06
#define OPTION_WAKEUP_TIME_2000	0x07
//FEC ����
#define OPTION_FEC_OFF			0x00	//�ر� FEC ���� Ĭ�ϣ�
#define OPTION_FEC_ON			0x01	//�� FEC ����
//���书��(��Լֵ)(dBm)
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


//�������ýṹ��
typedef struct {
	u8 head;//��ʾ������ͷ����Ϣ��ͨ�����ڱ�ʶ���������ͻ�����
	u8 addh;//��ȡĿ���ַ�ĸ�8λ
    u8 addl;//��ȡĿ���ַ�ĵ�8λ
	u8 sped;//����ͨ�ŵ����ʣ���������У��λ�������ʺͿ�������
	u8 chan;//Ŀ���ŵ�
	u8 option;//��������ͨ��ѡ��紫��ģʽ��IO������ʽ������ʱ�䡢FEC�������Լ����书�ʡ�
}Lora_WorkParam;

//�����ֲ�ͬ�ķ�ʽ���ʹ�������	
union LORA_WORKDATA{
	Lora_WorkParam lora_workprm;//ͨ���ṹ��ķ�ʽ���ʹ�������
	u8 buff[6];//ͨ��buff���鰴���ֽ���ʽ���ʹ�������
};//���ջ����͹�������


#endif
