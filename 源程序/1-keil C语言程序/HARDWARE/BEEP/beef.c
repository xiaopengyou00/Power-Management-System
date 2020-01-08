#include "beef.h"
#include "delay.h"

	    
//GPIO��ʼ��
void BEEF_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;			//PB12 -- ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//�����趨������ʼ��GPIOPB12
	GPIO_SetBits(GPIOB,GPIO_Pin_12);			//PB12 �����
}

//���������
void Beep_Call(void)
{
	BEEP = 0;
	delay_ms(100);
	BEEP = 1;
}
