#include "beef.h"
#include "relay.h"
#include "delay.h"
#include "timer.h"
#include "string.h"

unsigned short Time;			//倒计时时间
_Bool Timer_Flag = 1;				//"定时器"标志位
_Bool Control_Flag = 1;				//"倒计时"标志位
unsigned short Control_Times = 0;			//控制的次数

void TIM2_Int_Init(unsigned short arr,unsigned short psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//时钟使能
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr;			//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;			//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);			//使能指定的TIM3中断,允许更新中断
	//中断优先级NVIC设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;			//TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//抢占占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);			//初始化NVIC寄存器
	TIM_Cmd(TIM2, ENABLE);			//使能TIMx					 
}

//定时器2中断服务程序
void TIM2_IRQHandler(void)
{
	static unsigned short two = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)			//检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			//清除TIMx更新中断标志 
		two++;
		if(two >= 2)			//1s * 2 = 2s，给予一定时间用于采集数据
		{
			two = 0;
			Timer_Flag = 1;					//定时器标志位置位，即1s时间到
		}
		if(Control_Flag == 1)
		{
			Control_Times++;			//启用倒计时的次数
			if(Time > 0) Time--;			//倒计时开始
			if(Time == 0)			//倒计时结束
			{
				Control_Flag = 0;			//清除倒计时标志位
				Relay0 = 1; Relay1 = 1;Relay2 = 1;			//继电器全关
			}
		}
	}
}
