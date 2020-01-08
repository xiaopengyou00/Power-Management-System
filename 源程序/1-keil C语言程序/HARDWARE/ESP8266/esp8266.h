#ifndef	__ESP8266_H_
#define	__ESP8266_H_


typedef struct
{
	char staName[20];				//缓存SSID
	char staPass[40];				//缓存PassWord
	unsigned short err : 2; 		//错误类型
	unsigned short initStep : 4;	//初始化步骤
} NET_DEVICE_INFO;			//网络数据信息

extern NET_DEVICE_INFO netDeviceInfo;
extern unsigned char Status;			//服务是否被连接状态 0：已连接 2：无向应 3：未连接

_Bool Esp8266_Init(void);			//网络设备初始化函数
_Bool Esp8266_Net_SendData(unsigned char *string,unsigned char Len);			//网络设备发送数据
unsigned char Esp8266_WaitConnect(void);			//检查网络设备连接状态
unsigned char *Esp8266_GetIPD(unsigned short TimeOut);			//处理向ESP8266发来的数据函数  


#endif
