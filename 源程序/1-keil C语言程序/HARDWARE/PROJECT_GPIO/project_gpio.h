#ifndef _PROJECT_GPIO_H
#define _PROJECT_GPIO_H	 

#include "sys.h"


#define LED0	PCout(13)			//LED
#define JD2   PBout(6)			//�̵���
#define JD1   PBout(7)			//�̵���
#define JD0		PBout(8)			//�̵���
#define BEEP	PBout(12)			//������

void PROJECT_GPIO_Init(void);			//��ʼ��

		 				    
#endif
