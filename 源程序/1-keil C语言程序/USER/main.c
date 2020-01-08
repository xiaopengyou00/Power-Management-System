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
****����WIFI����ʵ��ʵ���ҵ�ԴԶ�̹���ϵͳ���****
*************************************************/


void CTR_Function1(unsigned char *);			//�������ݺ���1����
void CTR_Function2(float, float, float, unsigned char, unsigned char, unsigned char);			//�������ݺ���2����

//������
int main(void)
{	
	float A0, A1, A2;			//��·���Ե���ֵ
	unsigned int V;			//��ѹ
	float P0, P1, P2;			//��·���Թ���
	float Ak = 5.4;			//����kֵ
	float Vk = 321;			//��ѹKֵ
	unsigned char NET_Start = 0;			//��������ʼ��״̬ 0��δ��ʼ 1����ʼ 
	unsigned char Status = 3;			//�����Ƿ�����״̬ 0�������� 2������Ӧ 3��δ����
	unsigned char T_DATA[100];			//ESP8266��Ҫ���͵�����
	unsigned char * CTR_STR1 = NULL;			//ESP8266�յ�APP���������ݣ������ַ��
  
	BEEF_Init();			//��������ʼ��
	LED_Init();			//LED��ʼ��
	RELAY_Init();			//�̵�����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	USART1_Init(115200);			//����һ��ʼ��������Ϊ115200
	USART2_Init(115200);			//���ڶ���ʼ��������Ϊ115200
	TIM2_Int_Init(9999,7199);			//��ʱ����ʼ������ʱһ��
  ADC_Configuration();			//ADC��ʼ��
	delay_s(1);			//���ϳ�ʼ����������ʱ��
	
	//ϵͳ<--->APP
 	while(1)
	{			
		if(NET_Start == 1)				//ESP8266��ʼ���ɹ�
		{
			LED0 = 0;
			if(Status == 0)			//ϵͳ��APP��������״̬��ͨ������ͨ��
			{
				A0 = ((float)ADC_DMA_IN[0] * 3.3 / 4096) * Ak;			//����1
				A1 = ((float)ADC_DMA_IN[1] * 3.3 / 4096) * Ak;			//����2
				A2 = ((float)ADC_DMA_IN[2] * 3.3 / 4096) * Ak;			//����3
				if(A0 < 0.05) A0 = 0;
				if(A1 < 0.05) A1 = 0;
				if(A2 < 0.05) A2 = 0;
				V = ((float)ADC_DMA_IN[3] * 3.3 / 4096) * Vk;			//��ѹ
				P0 = A0 * V / 1000;			//����1
				P1 = A1 * V / 1000;			//����2
				P2 = A2 * V / 1000;			//����3
				Relay0_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8);			//�鿴�̵���1��״̬����Ϊ�ߵ�ƽΪ�أ���Ϊ�͵�ƽ��Ϊ��
				Relay1_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_7);			//�鿴�̵���2��״̬����Ϊ�ߵ�ƽΪ�أ���Ϊ�͵�ƽ��Ϊ��
				Relay2_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6);			//�鿴�̵���3��״̬����Ϊ�ߵ�ƽΪ�أ���Ϊ�͵�ƽ��Ϊ��			
				
				if(V < 10) V = 0;			//����Ҫ����220V��ѹ�����Ե���10V��ֱ����Ϊ0������������
				
				if(Timer_Flag == 1)			//���2s����һ������������APP������ϵ
				{
					//ϵͳ��Ҫ���͵�����
					sprintf((char *)T_DATA,"tT%hds\r\ntZA1:%.3fA\r\nP1:%.3fKW\r\nV:%dV\r\nzYA2:%.3fA\r\nP2:%.3fKW\r\nV:%dV\r\nyXA3:%.3fA\r\nP3:%.3fKW\r\nV:%dV\r\nx%d%d%d", \
						Time, A2, P2, V, A0, P0, V, A1, P1, V, Relay2_state, Relay0_state, Relay1_state);
					Esp8266_Net_SendData((unsigned char *)T_DATA, strlen((char *)T_DATA));			//�������ݵ�APP		
					Timer_Flag = 0;			//��ʱ����־λ��λ
				}
				CTR_STR1 = Esp8266_GetIPD(10);			//��ȡ���յ����� CTR_STR�Ѿ��Ǵ���֮������� ���磺+IPD,0,2:G2
				CTR_Function1(CTR_STR1);			//�����������1
				//CTR_Function2(P0, P1, P2, Relay0_state, Relay1_state, Relay2_state);			//�����������2
			}
			else
			{							
				Status = Esp8266_WaitConnect();			//��ȡ����״̬				
			}
		}
		else
		{
			LED0 =  1;
			if(!Esp8266_Init())			//ESP8266��ʼ������
			{
				NET_Start = 1;			//��ʼ���ɹ�
			}
		}
	}
}

//�������ݺ���1
void CTR_Function1(unsigned char *CTR_STR)
{
	unsigned short Hour, Minute, Second;			//���ö�ʱ���ص�ʱ��
	
	if(CTR_STR != NULL)
	{		
		if(strstr((char *)CTR_STR, "ON1") != NULL)			//�̵���1��
		{
			Relay0 = 0;
		}
		else if(strstr((char *)CTR_STR, "OFF1") != NULL)			//�̵���1��
		{
			Relay0 = 1;
		}
		else if(strstr((char *)CTR_STR, "ON2") != NULL)			//�̵���2��
		{
			Relay1 = 0; 
		}
		else if(strstr((char *)CTR_STR, "OFF2") != NULL)			//�̵���2��
		{
			Relay1 = 1; 
		}
		else if(strstr((char *)CTR_STR, "ON3") != NULL)			//�̵���3��
		{
			Relay2 = 0;
		}
		else if(strstr((char *)CTR_STR, "OFF3") != NULL)			//�̵���3��
		{
			Relay2 = 1; 
		}
		else if(strstr((char *)CTR_STR, "ON_ALL") != NULL)			//����ȫ���̵���
		{
			Relay0 = 0; Relay1 = 0; Relay2 = 0;
		}
		else if(strstr((char *)CTR_STR, "OFF_ALL") != NULL)			//�ر�ȫ���̵���
		{
			Relay0 = 1; Relay1 = 1; Relay2 = 1;
		}
		else if(strstr((char *)CTR_STR, "R") != NULL)			//�ж��Ƿ�Ϊ���ڷ���
		{
			return;
		}
		else if(strstr((char *)CTR_STR, "T") != NULL)			//�ж�Txx:xx:xx
		{
			char *CTR_Time = NULL;
			Beep_Call();
			CTR_Time = strstr((char *)CTR_STR, "T");			//�����ַ������״γ����ַ�'T'��λ��
			CTR_Time += 1;			//��ַ��һ�Ա��������ʱ��Ĳ���
			Hour = ((CTR_Time[0] - 0x30) * 10 * 3600) + ((CTR_Time[1] - 0x30) * 3600);			//Сʱ��ֵת��
			Minute = ((CTR_Time[3] - 0x30) * 10 * 60) + ((CTR_Time[4] - 0x30) * 60);			//������ֵת��
			Second = ((CTR_Time[6] - 0x30) * 10) + (CTR_Time[7] - 0x30);			//����ֵת��
			Time = Hour + Minute + Second;			//�ܵ���ʱʱ��
			if(Time > 0)
			{
				if(Time <= 5)
				{
					BEEP = 0;
					delay_ms(10);
					BEEP = 1;
				}
				Control_Flag = 1;			//�����ʱ����0����ʱ��ʼ
			}
		}
	}
}

//�������ݺ���2
void CTR_Function2(float P0, float P1, float P2, unsigned char Relay0_state, unsigned char Relay1_state, unsigned char Relay2_state)
{
	float P;			//��·�ܹ���
	
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
