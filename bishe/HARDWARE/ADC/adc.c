#include "adc.h"
#include "delay.h"
 
//��ʼ��ADC����ͨ��
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12M,ADC���ʱ�䲻�ܳ���14M

	//����ģ��ͨ����������
	//<�����ѹPB0-ADC_Channel_8>,<�������PB1-ADC_Channel_9>  �����ѹ�����ɼ�                      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	//<���ص�ѹPC0-ADC_Channel_10>,<���ص���PC1-ADC_Channel_11>  ���ص�ѹ�����ɼ�
	//<ֱ��ĸ�ߵ�ѹPC2-ADC_Channel_12>	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//<���ص���PA3-ADC_Channel_3>	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//<����������ѹPA4-ADC_Channel_4>,<������������PA5-ADC_Channel_5>  ˫��DCAC��ѹ�����ɼ�
	//<������ѹPA6-ADC_Channel_6>	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ,����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת���������������
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//ת����TIM2_CH2�ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת��ʱ��ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

//	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;  //ADC1�ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�1��
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
  		
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);//ת��ʱ��
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼   
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);//����ADC1�ж� 
//	ADC_ExternalTrigConvCmd(ADC1,ENABLE);    //�����ⲿ����ģʽʹ�ܣ�������ⲿ����ʵ��������//����ADCģ����ⲿ��ʵ���ϻ�����STM32�ڲ�)			  
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //�������������������	

}

//���ADCֵ ch:ͨ��
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ�䣬�б�Ҫ��Сת��ʱ�䣬����ʹ�����������
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_71Cycles5);//ADC1,ͨ��,˳�򣬲���ʱ��Ϊ239.5����	  			    
    //����ת��ʱ��=��������/ʱ�����ڣ���71.5+12.5��/12M=7us
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		  //ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
