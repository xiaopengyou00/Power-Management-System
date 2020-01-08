#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"


#define ADC1_DR_Address    ((uint32_t)0x4001244C) //ADC1这个外设的地址（查参考手册得出）

extern vu16 ADC_DMA_IN[4]; //供外部文件引用ADC文件变量

void ADC_DMA_Init(void);
void ADC_GPIO_Init(void);
void ADC_Configuration(void);
 

#endif 
