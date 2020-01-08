 #include "adc.h"
 #include "delay.h"
	   

vu16 ADC_DMA_IN[4];			//ADC数值存放的变量

//DMA初始化设置
void ADC_DMA_Init(void)
{ 
	DMA_InitTypeDef DMA_InitStructure;			//定义DMA初始化结构体
	DMA_DeInit(DMA1_Channel1);			//复位DMA通道1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			//定义 DMA通道外设基地址=ADC1_DR_Address
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_DMA_IN;			//定义DMA通道ADC数据存储器（其他函数可直接读此变量即是ADC值）
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;			//指定外设为源地址
	DMA_InitStructure.DMA_BufferSize = 4;			//定义DMA缓冲区大小（根据ADC采集通道数量修改）
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//当前外设寄存器地址不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//当前存储器地址：Disable不变，Enable递增（用于多通道采集）
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;			//定义外设数据宽度16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//定义存储器数据宽度16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;			//DMA通道操作模式位环形缓冲模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;			//DMA通道优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;			//禁止DMA通道存储器到存储器传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);			//初始化DMA通道1
	DMA_Cmd(DMA1_Channel1, ENABLE);			//使能DMA通道1
}

//GPIO初始化设置
void ADC_GPIO_Init(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure; 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);			//使能GPIOA时钟及ADC1时钟	   
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			//使能DMA时钟（用于ADC的数据传送）
	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//模拟输入引脚，电流
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//模拟输入引脚，电流
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//模拟输入引脚，电流
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//模拟输入引脚，电压
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//初始化ADC设置																   
void  ADC_Configuration(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure;			//定义ADC初始化结构体变量
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);			//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
	ADC_GPIO_Init();			//GPIO初始化设置
	ADC_DMA_Init();			//DMA初始化设置
	ADC_DeInit(ADC1);			//复位ADC1 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;			//模数转换工作在多通道扫描模式，DISABLE为单通道扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//模数转换工作在多次转换模式，DISABLE为单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;			//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;			//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 4;			//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);			//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期	 	 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADC1, ENABLE);			//开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
	ADC_Cmd(ADC1, ENABLE);			//使能指定的ADC1
	ADC_ResetCalibration(ADC1);			//使能复位校准  
	while(ADC_GetResetCalibrationStatus(ADC1));			//等待复位校准结束
	ADC_StartCalibration(ADC1);			//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));			//等待校准结束
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
} 
