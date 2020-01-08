#include "project_gpio.h"

	    
//GPIO初始化
void PROJECT_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);			//使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;			//PC13 -- LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			//根据设定参数初始化GPIOPC13
	GPIO_SetBits(GPIOC,GPIO_Pin_13);			//PC13 输出高

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;			//P6-8 -- 继电器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//根据设定参数初始化GPIOPB6-8
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);			//PB6-8 输出高

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;			//PB12 -- 蜂鸣器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//根据设定参数初始化GPIOPB12
	GPIO_SetBits(GPIOB,GPIO_Pin_12);			//PB12 输出高
}
