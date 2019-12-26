#include "adc.h"
#include "delay.h"
 
//初始化ADC规则通道
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12M,ADC最大时间不能超过14M

	//设置模拟通道输入引脚
	//<光伏电压PB0-ADC_Channel_8>,<光伏电流PB1-ADC_Channel_9>  光伏电压电流采集                      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	//<蓄电池电压PC0-ADC_Channel_10>,<蓄电池电流PC1-ADC_Channel_11>  蓄电池电压电流采集
	//<直流母线电压PC2-ADC_Channel_12>	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//<负载电流PA3-ADC_Channel_3>	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//<逆变器输出电压PA4-ADC_Channel_4>,<逆变器输出电流PA5-ADC_Channel_5>  双向DCAC电压电流采集
	//<电网电压PA6-ADC_Channel_6>	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式,独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件触发启动
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//转换由TIM2_CH2外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换时的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

//	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;  //ADC1中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
  		
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);//转换时间
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准   
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	ADC_StartCalibration(ADC1);	 //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);//开启ADC1中断 
//	ADC_ExternalTrigConvCmd(ADC1,ENABLE);    //设置外部触发模式使能（这个“外部“其实仅仅是相//对于ADC模块的外部，实际上还是在STM32内部)			  
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //设置软件触发启动功能	

}

//获得ADC值 ch:通道
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间，有必要减小转换时间，但会使采样误差增大
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_71Cycles5);//ADC1,通道,顺序，采样时间为239.5周期	  			    
    //采样转换时间=采样周期/时钟周期，（71.5+12.5）/12M=7us
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		  //使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
