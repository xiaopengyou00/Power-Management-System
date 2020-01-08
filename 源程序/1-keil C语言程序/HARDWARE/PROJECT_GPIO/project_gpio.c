#include "project_gpio.h"

	    
//GPIO��ʼ��
void PROJECT_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);			//ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;			//PC13 -- LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			//�����趨������ʼ��GPIOPC13
	GPIO_SetBits(GPIOC,GPIO_Pin_13);			//PC13 �����

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;			//P6-8 -- �̵���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//�����趨������ʼ��GPIOPB6-8
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);			//PB6-8 �����

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;			//PB12 -- ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//�����趨������ʼ��GPIOPB12
	GPIO_SetBits(GPIOB,GPIO_Pin_12);			//PB12 �����
}
