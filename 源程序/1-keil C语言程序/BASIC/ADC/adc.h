#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"


#define ADC1_DR_Address    ((uint32_t)0x4001244C) //ADC1�������ĵ�ַ����ο��ֲ�ó���

extern vu16 ADC_DMA_IN[4]; //���ⲿ�ļ�����ADC�ļ�����

void ADC_DMA_Init(void);
void ADC_GPIO_Init(void);
void ADC_Configuration(void);
 

#endif 
