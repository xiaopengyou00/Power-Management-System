#include "stm32f10x.h"
#include "net_usart.h"
#include "delay.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


NET_IO_INFO netIOInfo;

//�������ݺ���   str����Ҫ���͵�����   len�����ݳ���
void NET_IO_Send(unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)			//����һ֡����
	{
		USART_ClearFlag(NET_IO, USART_FLAG_TC );
		USART_SendData(NET_IO, *str++);
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);
	}
}

//�ȴ�������ɺ���   REV_OK-�������   REV_WAIT-���ճ�ʱδ���
_Bool NET_IO_WaitRecive(void)
{
	if(netIOInfo.dataLen == 0)			//������ռ���Ϊ0����˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)			//�����һ�ε�ֵ�������ͬ����˵���������
	{
		netIOInfo.dataLen = 0;			//��0���ռ���
		return REV_OK;			//���ؽ�����ɱ�־
	}
	netIOInfo.dataLenPre = netIOInfo.dataLen;			//��Ϊ��ͬ
	return REV_WAIT;			//���ؽ���δ��ɱ�־
}

//��ջ��溯��
void NET_IO_ClearRecive(void)
{
	netIOInfo.dataLen = 0;
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));
}

//�����жϺ���
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)			//�����ж�
	{
		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0;			//��ֹ���ڱ�ˢ��
		netIOInfo.buf[netIOInfo.dataLen++] = USART2->DR;
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}

//�������豸����һ��������ȴ���ȷ����Ӧ
//cmd����Ҫ���͵�����		res����Ҫ��������Ӧ
_Bool NET_DEVICE_SendCmd(char *cmd, char *res,unsigned int timeOut)
{
	NET_IO_Send((unsigned char *)cmd, strlen((const char *)cmd));			//д��������豸
	while(timeOut--)			//�ȴ�
	{
		if(NET_IO_WaitRecive() == REV_OK)			//����յ�����
		{
			if(strstr((const char *)netIOInfo.buf, res) != NULL)			//����������ؼ���
			{
				NET_IO_ClearRecive();			//��ջ���
				return 0;
			}
		}
		delay_ms(10);			//����ȴ�
	}
	return 1;
}

//��ʽ����ӡ
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	va_start(ap, fmt);
	vsprintf((char *)UsartPrintfBuf, fmt, ap);			//��ʽ��
	va_end(ap);
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

//���͹̶������ַ���
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)
	{
		USART_ClearFlag(NET_IO, USART_FLAG_TC);
		USART_SendData(USARTx, *str++);			//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);			//�ȴ��������
	}
}
