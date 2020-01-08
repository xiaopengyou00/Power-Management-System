#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


//供外部文件引用定时器文件变量
extern unsigned short Time;			//倒计时时间
extern _Bool Timer_Flag;				//"定时器"标志位
extern _Bool Control_Flag;				//"倒计时"标志位
extern unsigned short Control_Times;			//控制的次数

void TIM2_Int_Init(u16 arr,u16 psc);
	
	
#endif
