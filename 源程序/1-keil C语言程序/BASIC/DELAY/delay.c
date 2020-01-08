#include "delay.h"


#define AHB_INPUT  72			//�밴RCC�����õ�AHBʱ��Ƶ����д�������λMHz��

//uS΢�뼶��ʱ���򣨲ο�ֵ������ʱ����72MHzʱ���ֵ233015��	
void delay_us(u32 uS)			
{
	SysTick->LOAD=AHB_INPUT*uS;			//��װ������ֵ������Ƶ��72MHz��72��Ϊ1΢�룩
	SysTick->VAL=0x00;			//��ն�ʱ���ļ�����
	SysTick->CTRL=0x00000005;			//ʱ��ԴHCLK���򿪶�ʱ��
	while(!(SysTick->CTRL&0x00010000));			//�ȴ�������0
	SysTick->CTRL=0x00000004;			//�رն�ʱ��
}

//mS���뼶��ʱ���򣨲ο�ֵ������ʱ�������ֵ65535��	
void delay_ms(u16 ms)			
{	
	while( ms-- != 0)
	{
		delay_us(1000);			//����1000΢�����ʱ
	}
}
 
//S�뼶��ʱ���򣨲ο�ֵ������ʱ�������ֵ65535��
void delay_s(u16 s)				 		  
{	
	while( s-- != 0)
	{
		delay_ms(1000);			//����1000�������ʱ
	}
} 
