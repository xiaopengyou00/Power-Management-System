 #include "adc.h"
 #include "delay.h"
	   

vu16 ADC_DMA_IN[4];			//ADC��ֵ��ŵı���

//DMA��ʼ������
void ADC_DMA_Init(void)
{ 
	DMA_InitTypeDef DMA_InitStructure;			//����DMA��ʼ���ṹ��
	DMA_DeInit(DMA1_Channel1);			//��λDMAͨ��1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			//���� DMAͨ���������ַ=ADC1_DR_Address
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_DMA_IN;			//����DMAͨ��ADC���ݴ洢��������������ֱ�Ӷ��˱�������ADCֵ��
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;			//ָ������ΪԴ��ַ
	DMA_InitStructure.DMA_BufferSize = 4;			//����DMA��������С������ADC�ɼ�ͨ�������޸ģ�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//��ǰ����Ĵ�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//��ǰ�洢����ַ��Disable���䣬Enable���������ڶ�ͨ���ɼ���
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;			//�����������ݿ��16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//����洢�����ݿ��16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;			//DMAͨ������ģʽλ���λ���ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;			//DMAͨ�����ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;			//��ֹDMAͨ���洢�����洢������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);			//��ʼ��DMAͨ��1
	DMA_Cmd(DMA1_Channel1, ENABLE);			//ʹ��DMAͨ��1
}

//GPIO��ʼ������
void ADC_GPIO_Init(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure; 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);			//ʹ��GPIOAʱ�Ӽ�ADC1ʱ��	   
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			//ʹ��DMAʱ�ӣ�����ADC�����ݴ��ͣ�
	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//ģ���������ţ�����
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//ģ���������ţ�����
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//ģ���������ţ�����
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//ģ���������ţ���ѹ
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//��ʼ��ADC����																   
void  ADC_Configuration(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure;			//����ADC��ʼ���ṹ�����
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);			//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
	ADC_GPIO_Init();			//GPIO��ʼ������
	ADC_DMA_Init();			//DMA��ʼ������
	ADC_DeInit(ADC1);			//��λADC1 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;			//ģ��ת�������ڶ�ͨ��ɨ��ģʽ��DISABLEΪ��ͨ��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//ģ��ת�������ڶ��ת��ģʽ��DISABLEΪ����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;			//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;			//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 4;			//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);			//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   
	//����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	//ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ239.5����	 	 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADC1, ENABLE);			//����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
	ADC_Cmd(ADC1, ENABLE);			//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1);			//ʹ�ܸ�λУ׼  
	while(ADC_GetResetCalibrationStatus(ADC1));			//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);			//����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));			//�ȴ�У׼����
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
} 
