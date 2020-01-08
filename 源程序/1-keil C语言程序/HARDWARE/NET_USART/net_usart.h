#ifndef __NET_USART_H_
#define __NET_USART_H_

#include "stm32f10x.h"


//���������ݣ��ṹ��
typedef struct
{
	unsigned short dataLen;			//�������ݳ���
	unsigned short dataLenPre;			//��һ�εĳ������ݣ����ڱȽ�
	unsigned char buf[500];			//���ջ���
} NET_IO_INFO;

extern NET_IO_INFO netIOInfo;

#define REV_OK		0			//������ɱ�־
#define REV_WAIT	1			//����δ��ɱ�־
#define NET_IO		USART2

void NET_IO_Init(void);
void NET_IO_Send(unsigned char *str, unsigned short len);
_Bool NET_IO_WaitRecive(void);
void NET_IO_ClearRecive(void);
_Bool NET_DEVICE_SendCmd(char *cmd, char *res,unsigned int timeOut);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);


#endif
