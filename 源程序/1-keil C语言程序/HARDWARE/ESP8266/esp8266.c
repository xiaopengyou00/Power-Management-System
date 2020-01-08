#include "net_usart.h"
#include "esp8266.h"
#include "delay.h"
#include "string.h" 
#include "stdio.h" 

NET_DEVICE_INFO netDeviceInfo = {"", "", 0, 0};			//网络数据信息
unsigned char Status = 3;			//服务是否被连接状态 0：已连接 2：无向应 3：未连接

//网络设备初始化函数   返回: 0成功 1失败
_Bool Esp8266_Init(void)
{
	switch(netDeviceInfo.initStep)
	{
		case 0:	
			NET_IO_ClearRecive();
			NET_IO_Send((unsigned char *)"AT+GMR\r\n", strlen("AT+GMR\r\n"));
			delay_ms(150);
			printf( "\r\n**************************\r\n%s\r\n**************************\r\n", netIOInfo.buf);			//打印版本信息
			NET_IO_ClearRecive();			//清除串口
			netDeviceInfo.initStep=1;
		break;
		case 1:
			printf( "AP Tips:	AT\r\n");
			if(!NET_DEVICE_SendCmd("AT\r\n", "OK", 50))			//测试启动
				netDeviceInfo.initStep=2;
		break;
		case 2:
			printf( "AP Tips:	AT+RST\r\n");
			if(!NET_DEVICE_SendCmd("AT+RST\r\n", "OK", 50))			//重启模块
				netDeviceInfo.initStep=3;
			delay_ms(500);
		break;
		case 3:	
			printf( "AP Tips:	AT+CWMODE=2\r\n");
			if(!NET_DEVICE_SendCmd((char *)"AT+CWMODE=2\r\n", "OK", 50))			//设置为AP模式
				netDeviceInfo.initStep=4;
		break;
		case 4:	
			printf( "AP Tips:	AT+CIPMUX=1\r\n");
			if(!NET_DEVICE_SendCmd((char *)"AT+CIPMUX=1\r\n", "OK", 50))			//启动多连接
				netDeviceInfo.initStep=5;
		break;
		case 5:	
			printf( "AP Tips:	AT+CIPSERVER=1,888\r\n");
			if(!NET_DEVICE_SendCmd("AT+CIPSERVER=1,888\r\n", "OK", 50))			//开启服务器 地址为192.168.4.1 端口888
				netDeviceInfo.initStep=6;
		break;
	}
	if(netDeviceInfo.initStep == 6)
		return 0;			//完成连接所需的六个步骤，即成功连接初始化ESP8266，返回0
	else
		return 1;			//ESP8266初始化失败
}

//网络设备发送数据   data：需要发送的数据   len：数据长度
_Bool Esp8266_Net_SendData(unsigned char *string, unsigned char Len)
{
	unsigned char Cmd[40];
	memset(Cmd, 0, sizeof(Cmd));
	sprintf((char*)Cmd, "AT+CIPSEND=0,%d\r\n", Len);
	if(NET_DEVICE_SendCmd((char*)Cmd, ">", 50)) return 1;
	if(NET_DEVICE_SendCmd((char*)string, "SEND OK", 50)) return 1;	
	return 0;
}

//检查网络设备连接状态
unsigned char Esp8266_WaitConnect(void)
{
	
	unsigned char TimeOut = 100;
	NET_IO_Send((unsigned char *)"AT+CIPSTATUS\r\n", strlen("AT+CIPSTATUS\r\n"));
	while(--TimeOut)
	{
		if(NET_IO_WaitRecive() == 0)
		{
			if(strstr((const char *)netIOInfo.buf, "+CIPSTATUS:"))			//如果有cilent连接 会返回+CIPSTATUS: IP和端口 
				Status = 0;			//已连接
			else
				Status = 3;			//未连接
			break;		
		}
		delay_ms(2);
	}
	if(TimeOut == 0)			//无响应
	{
		Status= 2;					
	}
	return Status;
}

//处理向ESP8266发来的数据函数  
unsigned char *Esp8266_GetIPD(unsigned short TimeOut)
{
	char *PtrIPD = NULL;
	do
	{
		if(NET_IO_WaitRecive() == 0)			//返回OK
		{		
			PtrIPD = strstr((char *)netIOInfo.buf, "IPD");			//搜索“IPD”头
			if(PtrIPD == NULL)			//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				UsartPrintf(USART1, "\"IPD\" not found\r\n");
			}
			else
			{
				PtrIPD=strchr(PtrIPD, ':');			//查找字符串':'中首次出现字符ptrIPD的位置。
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
