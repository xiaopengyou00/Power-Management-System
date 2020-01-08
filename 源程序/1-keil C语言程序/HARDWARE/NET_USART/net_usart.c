#include "stm32f10x.h"
#include "net_usart.h"
#include "delay.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


NET_IO_INFO netIOInfo;

//发送数据函数   str：需要发送的数据   len：数据长度
void NET_IO_Send(unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)			//发送一帧数据
	{
		USART_ClearFlag(NET_IO, USART_FLAG_TC );
		USART_SendData(NET_IO, *str++);
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);
	}
}

//等待接收完成函数   REV_OK-接收完成   REV_WAIT-接收超时未完成
_Bool NET_IO_WaitRecive(void)
{
	if(netIOInfo.dataLen == 0)			//如果接收计数为0，则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)			//如果上一次的值和这次相同，则说明接收完毕
	{
		netIOInfo.dataLen = 0;			//清0接收计数
		return REV_OK;			//返回接收完成标志
	}
	netIOInfo.dataLenPre = netIOInfo.dataLen;			//置为相同
	return REV_WAIT;			//返回接收未完成标志
}

//清空缓存函数
void NET_IO_ClearRecive(void)
{
	netIOInfo.dataLen = 0;
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));
}

//接收中断函数
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)			//接收中断
	{
		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0;			//防止串口被刷爆
		netIOInfo.buf[netIOInfo.dataLen++] = USART2->DR;
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}

//向网络设备发送一条命令，并等待正确的响应
//cmd：需要发送的命令		res：需要检索的响应
_Bool NET_DEVICE_SendCmd(char *cmd, char *res,unsigned int timeOut)
{
	NET_IO_Send((unsigned char *)cmd, strlen((const char *)cmd));			//写命令到网络设备
	while(timeOut--)			//等待
	{
		if(NET_IO_WaitRecive() == REV_OK)			//如果收到数据
		{
			if(strstr((const char *)netIOInfo.buf, res) != NULL)			//如果检索到关键词
			{
				NET_IO_ClearRecive();			//清空缓存
				return 0;
			}
		}
		delay_ms(10);			//挂起等待
	}
	return 1;
}

//格式化打印
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	va_start(ap, fmt);
	vsprintf((char *)UsartPrintfBuf, fmt, ap);			//格式化
	va_end(ap);
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

//发送固定长度字符串
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)
	{
		USART_ClearFlag(NET_IO, USART_FLAG_TC);
		USART_SendData(USARTx, *str++);			//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);			//等待发送完成
	}
}
