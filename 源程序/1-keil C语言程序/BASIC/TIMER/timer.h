#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


//���ⲿ�ļ����ö�ʱ���ļ�����
extern unsigned short Time;			//����ʱʱ��
extern _Bool Timer_Flag;				//"��ʱ��"��־λ
extern _Bool Control_Flag;				//"����ʱ"��־λ
extern unsigned short Control_Times;			//���ƵĴ���

void TIM2_Int_Init(u16 arr,u16 psc);
	
	
#endif
