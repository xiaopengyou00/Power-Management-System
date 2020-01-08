#include "beef.h"
#include "relay.h"
#include "delay.h"
#include "timer.h"
#include "string.h"

unsigned short Time;			//����ʱʱ��
_Bool Timer_Flag = 1;				//"��ʱ��"��־λ
_Bool Control_Flag = 1;				//"����ʱ"��־λ
unsigned short Control_Times = 0;			//���ƵĴ���

void TIM2_Int_Init(unsigned short arr,unsigned short psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//ʱ��ʹ��
	//��ʱ��TIM2��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr;			//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;			//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);			//ʹ��ָ����TIM3�ж�,��������ж�
	//�ж����ȼ�NVIC����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;			//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//��ռռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);			//��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM2, ENABLE);			//ʹ��TIMx					 
}

//��ʱ��2�жϷ������
void TIM2_IRQHandler(void)
{
	static unsigned short two = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)			//���TIM2�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			//���TIMx�����жϱ�־ 
		two++;
		if(two >= 2)			//1s * 2 = 2s������һ��ʱ�����ڲɼ�����
		{
			two = 0;
			Timer_Flag = 1;					//��ʱ����־λ��λ����1sʱ�䵽
		}
		if(Control_Flag == 1)
		{
			Control_Times++;			//���õ���ʱ�Ĵ���
			if(Time > 0) Time--;			//����ʱ��ʼ
			if(Time == 0)			//����ʱ����
			{
				Control_Flag = 0;			//�������ʱ��־λ
				Relay0 = 1; Relay1 = 1;Relay2 = 1;			//�̵���ȫ��
			}
		}
	}
}
