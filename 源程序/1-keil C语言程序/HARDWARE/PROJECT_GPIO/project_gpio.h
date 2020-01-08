#ifndef _PROJECT_GPIO_H
#define _PROJECT_GPIO_H	 

#include "sys.h"


#define LED0	PCout(13)			//LED
#define JD2   PBout(6)			//继电器
#define JD1   PBout(7)			//继电器
#define JD0		PBout(8)			//继电器
#define BEEP	PBout(12)			//蜂鸣器

void PROJECT_GPIO_Init(void);			//初始化

		 				    
#endif
