#include "beef.h"
#include "delay.h"

	    
//GPIO初始化
void BEEF_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;			//PB12 -- 蜂鸣器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//根据设定参数初始化GPIOPB12
	GPIO_SetBits(GPIOB,GPIO_Pin_12);			//PB12 输出高
}

//激活蜂鸣器
void Beep_Call(void)
{
	BEEP = 0;
	delay_ms(100);
	BEEP = 1;
}
