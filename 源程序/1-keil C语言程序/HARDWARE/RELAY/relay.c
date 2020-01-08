#include "relay.h"

unsigned char Relay0_state;			//继电器1的状态
unsigned char Relay1_state;			//继电器2的状态
unsigned char Relay2_state;			//继电器3的状态

//GPIO初始化
void RELAY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;			//P6-8 -- 继电器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//根据设定参数初始化GPIOPB6-8
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);			//PB6-8 输出高
}
