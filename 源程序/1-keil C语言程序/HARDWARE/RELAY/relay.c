#include "relay.h"

unsigned char Relay0_state;			//�̵���1��״̬
unsigned char Relay1_state;			//�̵���2��״̬
unsigned char Relay2_state;			//�̵���3��״̬

//GPIO��ʼ��
void RELAY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;			//P6-8 -- �̵���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//�����趨������ʼ��GPIOPB6-8
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);			//PB6-8 �����
}
