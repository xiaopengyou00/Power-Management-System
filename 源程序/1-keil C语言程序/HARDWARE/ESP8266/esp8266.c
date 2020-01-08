#include "net_usart.h"
#include "esp8266.h"
#include "delay.h"
#include "string.h" 
#include "stdio.h" 

NET_DEVICE_INFO netDeviceInfo = {"", "", 0, 0};			//����������Ϣ
unsigned char Status = 3;			//�����Ƿ�����״̬ 0�������� 2������Ӧ 3��δ����

//�����豸��ʼ������   ����: 0�ɹ� 1ʧ��
_Bool Esp8266_Init(void)
{
	switch(netDeviceInfo.initStep)
	{
		case 0:	
			NET_IO_ClearRecive();
			NET_IO_Send((unsigned char *)"AT+GMR\r\n", strlen("AT+GMR\r\n"));
			delay_ms(150);
			printf( "\r\n**************************\r\n%s\r\n**************************\r\n", netIOInfo.buf);			//��ӡ�汾��Ϣ
			NET_IO_ClearRecive();			//�������
			netDeviceInfo.initStep=1;
		break;
		case 1:
			printf( "AP Tips:	AT\r\n");
			if(!NET_DEVICE_SendCmd("AT\r\n", "OK", 50))			//��������
				netDeviceInfo.initStep=2;
		break;
		case 2:
			printf( "AP Tips:	AT+RST\r\n");
			if(!NET_DEVICE_SendCmd("AT+RST\r\n", "OK", 50))			//����ģ��
				netDeviceInfo.initStep=3;
			delay_ms(500);
		break;
		case 3:	
			printf( "AP Tips:	AT+CWMODE=2\r\n");
			if(!NET_DEVICE_SendCmd((char *)"AT+CWMODE=2\r\n", "OK", 50))			//����ΪAPģʽ
				netDeviceInfo.initStep=4;
		break;
		case 4:	
			printf( "AP Tips:	AT+CIPMUX=1\r\n");
			if(!NET_DEVICE_SendCmd((char *)"AT+CIPMUX=1\r\n", "OK", 50))			//����������
				netDeviceInfo.initStep=5;
		break;
		case 5:	
			printf( "AP Tips:	AT+CIPSERVER=1,888\r\n");
			if(!NET_DEVICE_SendCmd("AT+CIPSERVER=1,888\r\n", "OK", 50))			//���������� ��ַΪ192.168.4.1 �˿�888
				netDeviceInfo.initStep=6;
		break;
	}
	if(netDeviceInfo.initStep == 6)
		return 0;			//�������������������裬���ɹ����ӳ�ʼ��ESP8266������0
	else
		return 1;			//ESP8266��ʼ��ʧ��
}

//�����豸��������   data����Ҫ���͵�����   len�����ݳ���
_Bool Esp8266_Net_SendData(unsigned char *string, unsigned char Len)
{
	unsigned char Cmd[40];
	memset(Cmd, 0, sizeof(Cmd));
	sprintf((char*)Cmd, "AT+CIPSEND=0,%d\r\n", Len);
	if(NET_DEVICE_SendCmd((char*)Cmd, ">", 50)) return 1;
	if(NET_DEVICE_SendCmd((char*)string, "SEND OK", 50)) return 1;	
	return 0;
}

//��������豸����״̬
unsigned char Esp8266_WaitConnect(void)
{
	
	unsigned char TimeOut = 100;
	NET_IO_Send((unsigned char *)"AT+CIPSTATUS\r\n", strlen("AT+CIPSTATUS\r\n"));
	while(--TimeOut)
	{
		if(NET_IO_WaitRecive() == 0)
		{
			if(strstr((const char *)netIOInfo.buf, "+CIPSTATUS:"))			//�����cilent���� �᷵��+CIPSTATUS: IP�Ͷ˿� 
				Status = 0;			//������
			else
				Status = 3;			//δ����
			break;		
		}
		delay_ms(2);
	}
	if(TimeOut == 0)			//����Ӧ
	{
		Status= 2;					
	}
	return Status;
}

//������ESP8266���������ݺ���  
unsigned char *Esp8266_GetIPD(unsigned short TimeOut)
{
	char *PtrIPD = NULL;
	do
	{
		if(NET_IO_WaitRecive() == 0)			//����OK
		{		
			PtrIPD = strstr((char *)netIOInfo.buf, "IPD");			//������IPD��ͷ
			if(PtrIPD == NULL)			//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				UsartPrintf(USART1, "\"IPD\" not found\r\n");
			}
			else
			{
				PtrIPD=strchr(PtrIPD, ':');			//�����ַ���':'���״γ����ַ�ptrIPD��λ�á�
				if(PtrIPD != NULL)
				{
					PtrIPD++;
					printf("%s\r",PtrIPD);
						return (unsigned char *)(PtrIPD);
				}
				else
					return NULL;
			}	
		}
		delay_ms(5);	
	}while(TimeOut--);
	return NULL;
}
