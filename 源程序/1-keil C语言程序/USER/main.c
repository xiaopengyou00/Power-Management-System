#include "led.h"
#include "delay.h"
#include "relay.h"
#include "sys.h"
#include "usart.h"	 
#include "net_usart.h"		
#include "esp8266.h"
#include "timer.h"
#include "adc.h"
#include "beef.h"
#include "string.h" 
/*************************************************
****利用WIFI技术实现实验室电源远程管理系统设计****
*************************************************/


void CTR_Function1(unsigned char *);			//处理数据函数1声明
void CTR_Function2(float, float, float, unsigned char, unsigned char, unsigned char);			//处理数据函数2声明

//主函数
int main(void)
{	
	float A0, A1, A2;			//三路各自电流值
	unsigned int V;			//电压
	float P0, P1, P2;			//三路各自功率
	float Ak = 5.4;			//电流k值
	float Vk = 321;			//电压K值
	unsigned char NET_Start = 0;			//服务器初始化状态 0：未开始 1：开始 
	unsigned char Status = 3;			//服务是否被连接状态 0：已连接 2：无向应 3：未连接
	unsigned char T_DATA[100];			//ESP8266所要发送的数据
	unsigned char * CTR_STR1 = NULL;			//ESP8266收到APP发来的数据（缓存地址）
  
	BEEF_Init();			//蜂鸣器初始化
	LED_Init();			//LED初始化
	RELAY_Init();			//继电器初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	USART1_Init(115200);			//串口一初始化波特率为115200
	USART2_Init(115200);			//串口二初始化波特率为115200
	TIM2_Int_Init(9999,7199);			//定时器初始化，定时一秒
  ADC_Configuration();			//ADC初始化
	delay_s(1);			//以上初始化就绪所需时间
	
	//系统<--->APP
 	while(1)
	{			
		if(NET_Start == 1)				//ESP8266初始化成功
		{
			LED0 = 0;
			if(Status == 0)			//系统与APP处于连接状态，通过串口通信
			{
				A0 = ((float)ADC_DMA_IN[0] * 3.3 / 4096) * Ak;			//电流1
				A1 = ((float)ADC_DMA_IN[1] * 3.3 / 4096) * Ak;			//电流2
				A2 = ((float)ADC_DMA_IN[2] * 3.3 / 4096) * Ak;			//电流3
				if(A0 < 0.05) A0 = 0;
				if(A1 < 0.05) A1 = 0;
				if(A2 < 0.05) A2 = 0;
				V = ((float)ADC_DMA_IN[3] * 3.3 / 4096) * Vk;			//电压
				P0 = A0 * V / 1000;			//功率1
				P1 = A1 * V / 1000;			//功率2
				P2 = A2 * V / 1000;			//功率3
				Relay0_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8);			//查看继电器1的状态，若为高电平为关，若为低电平则为开
				Relay1_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_7);			//查看继电器2的状态，若为高电平为关，若为低电平则为开
				Relay2_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6);			//查看继电器3的状态，若为高电平为关，若为低电平则为开			
				
				if(V < 10) V = 0;			//因需要接入220V电压，所以低于10V的直接置为0以免除后面计算
				
				if(Timer_Flag == 1)			//间隔2s发送一条数据用于与APP保持联系
				{
					//系统所要发送的数据
					sprintf((char *)T_DATA,"tT%hds\r\ntZA1:%.3fA\r\nP1:%.3fKW\r\nV:%dV\r\nzYA2:%.3fA\r\nP2:%.3fKW\r\nV:%dV\r\nyXA3:%.3fA\r\nP3:%.3fKW\r\nV:%dV\r\nx%d%d%d", \
						Time, A2, P2, V, A0, P0, V, A1, P1, V, Relay2_state, Relay0_state, Relay1_state);
					Esp8266_Net_SendData((unsigned char *)T_DATA, strlen((char *)T_DATA));			//发送数据到APP		
					Timer_Flag = 0;			//定时器标志位复位
				}
				CTR_STR1 = Esp8266_GetIPD(10);			//获取接收到数据 CTR_STR已经是处理之后的数据 例如：+IPD,0,2:G2
				CTR_Function1(CTR_STR1);			//处理相关数据1
				//CTR_Function2(P0, P1, P2, Relay0_state, Relay1_state, Relay2_state);			//处理相关数据2
			}
			else
			{							
				Status = Esp8266_WaitConnect();			//获取连接状态				
			}
		}
		else
		{
			LED0 =  1;
			if(!Esp8266_Init())			//ESP8266初始化配置
			{
				NET_Start = 1;			//初始化成功
			}
		}
	}
}

//处理数据函数1
void CTR_Function1(unsigned char *CTR_STR)
{
	unsigned short Hour, Minute, Second;			//设置定时开关的时间
	
	if(CTR_STR != NULL)
	{		
		if(strstr((char *)CTR_STR, "ON1") != NULL)			//继电器1开
		{
			Relay0 = 0;
		}
		else if(strstr((char *)CTR_STR, "OFF1") != NULL)			//继电器1关
		{
			Relay0 = 1;
		}
		else if(strstr((char *)CTR_STR, "ON2") != NULL)			//继电器2开
		{
			Relay1 = 0; 
		}
		else if(strstr((char *)CTR_STR, "OFF2") != NULL)			//继电器2关
		{
			Relay1 = 1; 
		}
		else if(strstr((char *)CTR_STR, "ON3") != NULL)			//继电器3开
		{
			Relay2 = 0;
		}
		else if(strstr((char *)CTR_STR, "OFF3") != NULL)			//继电器3关
		{
			Relay2 = 1; 
		}
		else if(strstr((char *)CTR_STR, "ON_ALL") != NULL)			//开启全部继电器
		{
			Relay0 = 0; Relay1 = 0; Relay2 = 0;
		}
		else if(strstr((char *)CTR_STR, "OFF_ALL") != NULL)			//关闭全部继电器
		{
			Relay0 = 1; Relay1 = 1; Relay2 = 1;
		}
		else if(strstr((char *)CTR_STR, "R") != NULL)			//判断是否为周期发送
		{
			return;
		}
		else if(strstr((char *)CTR_STR, "T") != NULL)			//判断Txx:xx:xx
		{
			char *CTR_Time = NULL;
			Beep_Call();
			CTR_Time = strstr((char *)CTR_STR, "T");			//查找字符串中首次出现字符'T'的位置
			CTR_Time += 1;			//地址加一以便后续计算时间的操作
			Hour = ((CTR_Time[0] - 0x30) * 10 * 3600) + ((CTR_Time[1] - 0x30) * 3600);			//小时数值转换
			Minute = ((CTR_Time[3] - 0x30) * 10 * 60) + ((CTR_Time[4] - 0x30) * 60);			//分钟数值转换
			Second = ((CTR_Time[6] - 0x30) * 10) + (CTR_Time[7] - 0x30);			//秒数值转换
			Time = Hour + Minute + Second;			//总倒计时时间
			if(Time > 0)
			{
				if(Time <= 5)
				{
					BEEP = 0;
					delay_ms(10);
					BEEP = 1;
				}
				Control_Flag = 1;			//如果定时大于0，则定时开始
			}
		}
	}
}

//处理数据函数2
void CTR_Function2(float P0, float P1, float P2, unsigned char Relay0_state, unsigned char Relay1_state, unsigned char Relay2_state)
{
	float P;			//三路总功率
	
	P = P0 + P1 + P2;
	if(P > 0.75)
	{
		BEEP = 0;
		delay_ms(20);
		BEEP = 1;
		BEEP = 0;
		delay_ms(20);
		BEEP = 1;
	}
	
	if((P0 == 0) && (Relay0_state == 0))
	{
		BEEP = 0;
		delay_s(1);
		BEEP = 1;
	}else if((P1 == 0) && (Relay1_state == 0))
	{
		BEEP = 0;
		delay_s(1);
		BEEP = 1;
	}
	else if((P2 == 0) && (Relay2_state == 0))
	{
		BEEP = 0;
		delay_s(1);
		BEEP = 1;
	}
}
