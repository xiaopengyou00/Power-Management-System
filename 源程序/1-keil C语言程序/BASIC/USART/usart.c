#include "sys.h"
#include "usart.h"


unsigned char i = 0;			//数组指针
unsigned char USART_Start = 0;		//串口接收标志
unsigned char USART_REC_Finish_FLAG = 0;		//串口接收完成标志
unsigned char REC_Char[100];			//缓冲区
unsigned char CTR_STR2[100];			//串口收到Qt发来的数据
			 
//使UASRT串口可用printf函数发送
//在usart.h文件里可更换使用printf函数的串口号	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{
	int handle; 
}; 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x)
{	
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{	
	while((USART_n->SR&0X40)==0);			//循环发送,直到发送完毕   
  USART_n->DR = (u8) ch;      
	return ch;
}
#endif 


/*
USART1串口相关程序
*/
#if EN_USART1			//USART1使用与屏蔽选择
u8 USART1_RX_BUF[USART1_REC_LEN];			//接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;			//接收状态标记	  

/*
USART1专用的printf函数
当同时开启2个以上串口时，printf函数只能用于其中之一，其他串口要自创独立的printf函数
调用方法：USART1_printf("123"); //向USART2发送字符123
*/
void USART1_printf (char *fmt, ...)
{	
	char buffer[USART1_REC_LEN+1];			//数据长度
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART1_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART1_REC_LEN) && (i < strlen(buffer)))
	{
		USART_SendData(USART1, (u8) buffer[i++]);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

//串口1初始化并启动
void USART1_Init(u32 bound) 
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);			//使能USART1，GPIOA时钟
	//USART1_TX   PA9初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	//USART1_RX	  PA10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;			//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//收发模式
  USART_Init(USART1, &USART_InitStructure);			//初始化串口
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//开启ENABLE/关闭DISABLE中断
  USART_Cmd(USART1, ENABLE);			//使能串口 
	//Usart1 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);			//根据指定的参数初始化VIC寄存器 
}

void USART1_IRQHandler(void)			//串口1中断服务程序（固定的函数名不能修改）
{	
	u8 j=0; 
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE) == SET) 
	{
		if(USART_ReceiveData(USART1) == 'S') USART_Start = 1;			//串口通信开始标志置1
		if(USART_ReceiveData(USART1) == 's') USART_Start = 0;			//串口通信开始标志置1
		if(USART_ReceiveData(USART1) == '\t')			//结束标志位
		{ 
			i = 0;
			USART_REC_Finish_FLAG = 1;
			for(j = 0;j < 100;j++)
			REC_Char[j] = '\0';
		}
		else
		{
			if(i == 0)
			{
				for(j = 0;j < 100;j++)
				CTR_STR2[j] = '\0';
			}	
			REC_Char[i] = USART_ReceiveData(USART1);
			CTR_STR2[i]= REC_Char[i];
			i++;
		}
	}
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{ 
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//清除接收中断标志	
	} 
} 
#endif	

/*
USART2串口相关程序
*/
#if EN_USART2			//USART2使用与屏蔽选择
u8 USART2_RX_BUF[USART2_REC_LEN];			//接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;			//接收状态标记	  

/*
USART2专用的printf函数
当同时开启2个以上串口时，printf函数只能用于其中之一，其他串口要自创独立的printf函数
调用方法：USART2_printf("123");			//向USART2发送字符123
*/
void USART2_printf (char *fmt, ...)
{	
	char buffer[USART2_REC_LEN+1];			//数据长度
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART2_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART2_REC_LEN) && (i < strlen(buffer)))
	{
		USART_SendData(USART2, (u8) buffer[i++]);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

//串口2初始化并启动
void USART2_Init(u32 bound)
{ 
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);		//使能UART2所在GPIOA的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		//使能串口的RCC时钟
	//USART2_TX   PA2初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	//USART2_RX   PA3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;			//一般设置为9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//收发模式
  USART_Init(USART2, &USART_InitStructure);			//初始化串口
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//开启ENABLE/关闭DISABLE中断
  USART_Cmd(USART2, ENABLE);			//使能串口 
  //Usart2 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);			//根据指定的参数初始化VIC寄存器 
}

//串口2中断服务程序（固定的函数名不能修改）	
//void USART2_IRQHandler(void)
//{ 
//	
//} 
#endif	

/*
USART3串口相关程序
*/
#if EN_USART3			//如果使能了接收
u8 USART3_RX_BUF[USART3_REC_LEN];			//接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;			//接收状态标记	  

/*
USART3专用的printf函数
当同时开启2个以上串口时，printf函数只能用于其中之一，其他串口要自创独立的printf函数
调用方法：USART3_printf("123"); //向USART3发送字符123
*/
void USART3_printf (char *fmt, ...)
{	
	char buffer[USART3_REC_LEN+1];			//数据长度
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART3_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART3_REC_LEN) && (i < strlen(buffer)))
	{
		USART_SendData(USART3, (u8) buffer[i++]);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

void USART3_Init(u32 BaudRate)			//USART3初始化并启动
{
	//GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure; 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);			//使能UART3所在GPIOB的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);			//使能串口的RCC时钟
	//USART3_TX   PB10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //USART3_RX   PB11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //USART3 初始化设置
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//收发模式
  USART_Init(USART3, &USART_InitStructure);			//配置串口3
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);			//使能串口接收中断  
  USART_Cmd(USART3, ENABLE);			//使能串口3
	//Usart3 NVIC 配置
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//允许USART3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);			//根据指定的参数初始化VIC寄存器 
}

//串口3中断服务程序（固定的函数名不能修改）
void USART3_IRQHandler(void)
{ 	

}
#endif	
