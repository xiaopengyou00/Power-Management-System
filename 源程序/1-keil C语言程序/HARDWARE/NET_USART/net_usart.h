#ifndef __NET_USART_H_
#define __NET_USART_H_

#include "stm32f10x.h"


//（接受数据）结构体
typedef struct
{
	unsigned short dataLen;			//接收数据长度
	unsigned short dataLenPre;			//上一次的长度数据，用于比较
	unsigned char buf[500];			//接收缓存
} NET_IO_INFO;

extern NET_IO_INFO netIOInfo;

#define REV_OK		0			//接收完成标志
#define REV_WAIT	1			//接收未完成标志
#define NET_IO		USART2

void NET_IO_Init(void);
void NET_IO_Send(unsigned char *str, unsigned short len);
_Bool NET_IO_WaitRecive(void);
void NET_IO_ClearRecive(void);
_Bool NET_DEVICE_SendCmd(char *cmd, char *res,unsigned int timeOut);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);


#endif
