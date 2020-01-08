#ifndef	__ESP8266_H_
#define	__ESP8266_H_


typedef struct
{
	char staName[20];				//����SSID
	char staPass[40];				//����PassWord
	unsigned short err : 2; 		//��������
	unsigned short initStep : 4;	//��ʼ������
} NET_DEVICE_INFO;			//����������Ϣ

extern NET_DEVICE_INFO netDeviceInfo;
extern unsigned char Status;			//�����Ƿ�����״̬ 0�������� 2������Ӧ 3��δ����

_Bool Esp8266_Init(void);			//�����豸��ʼ������
_Bool Esp8266_Net_SendData(unsigned char *string,unsigned char Len);			//�����豸��������
unsigned char Esp8266_WaitConnect(void);			//��������豸����״̬
unsigned char *Esp8266_GetIPD(unsigned short TimeOut);			//������ESP8266���������ݺ���  


#endif
