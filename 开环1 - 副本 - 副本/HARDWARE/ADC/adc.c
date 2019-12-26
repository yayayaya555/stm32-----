 #include "adc.h"
 #include "delay.h"
 #include "usart.h"
 
//初始化ADC
//以规则通道为例

u8 Current_num=0;
u16 ALL_Current_Sample=0, num=1;
extern u16 Current_Average;
char USART_SEND[6]="    \r\n";
extern u8 Usart_Send_flag;

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12M,ADC最大时间不能超过14M

	//设置模拟通道输入引脚
	//<PA0-ADC_Channel_0>,<PA1-ADC_Channel_1>,<PA2-ADC_Channel_2>,<PA3-ADC_Channel_3>                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式,独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//转换由TIM2_CH2外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换时的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;  //ADC1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
  		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);//转换时间
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准   
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	ADC_StartCalibration(ADC1);	 //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);//开启ADC1中断 
	ADC_ExternalTrigConvCmd(ADC1,ENABLE);    //设置外部触发模式使能（这个“外部“其实仅仅是相//对于ADC模块的外部，实际上还是在STM32内部)			  
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //设置软件触发启动功能	

}

void ADC1_2_IRQHandler(void)
{

	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) !=RESET)//检查指定的ADC中断是否发生
	{
//		if(Current_num<4)
//		{
//			ALL_Current_Sample+=ADC_GetConversionValue(ADC1);//返回最近一次ADC1规则组的转换结果
//			Current_num++;					
//	    }
//		if(Current_num==4)
//		{
//			Current_Average=ALL_Current_Sample/4;
//			if((Usart_Send_flag==1)&&(num<=2500))
//			{	
//				USART();
//				//printf("%d\r\n",Current_Average);	//打印数字量
//				num++;
//				if(num==2501)
//				{	
//					Usart_Send_flag=0;
//                    num=1;
//				}					
//			}
//			Current_num=0;
//			ALL_Current_Sample=0;
//		}
			if((Usart_Send_flag==1)&&(num<=2500))
			{	
				USART();
				//printf("%d\r\n",Current_Average);	//打印数字量
				num++;
				if(num==2501)
				{	
					Usart_Send_flag=0;
                    num=1;
				}					
			}
		
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC); //手动清除ADC1的中断待处理位
    }
}

void USART(void)
{
	u8 t;
	u16 xxxx;
	xxxx=ADC_GetConversionValue(ADC1);
	USART_SEND[0]=xxxx/1000+'0';
	USART_SEND[1]=xxxx%1000/100+'0';
	USART_SEND[2]=xxxx%100/10+'0';
	USART_SEND[3]=xxxx%10+'0';
//	USART_SEND[0]=Current_Average/1000+'0';
//	USART_SEND[1]=Current_Average%1000/100+'0';
//	USART_SEND[2]=Current_Average%100/10+'0';
//	USART_SEND[3]=Current_Average%10+'0';

	for(t=0;t<6;t++)
	{
		USART1->DR=USART_SEND[t];
		while((USART1->SR&0X40)==0);//等待发送结束
	}
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

//冒泡法排序数字滤波
#define N 12
u16 Get_Adc_Average(u8 ch)
{
	u32 sum=0;
	u16 value_buf[N],temp;
	u8 t,i,j,count;
	
	for(t=0;t<N;t++)
	{
		value_buf[t]=Get_Adc(ch);
		delay_ms(5);
	}
	for (j=0;j<N-1;j++)  
	{  
		for (i=0;i<N-j;i++)  
		{  
			if ( value_buf[i]>value_buf[i+1] )  
			{  
				temp = value_buf[i];  
				value_buf[i] = value_buf[i+1];  
				value_buf[i+1] = temp;  
			}  
		}  
	} 
	for(count=1;count<N-1;count++)  
		sum += value_buf[count]; 
	return (u16)(sum/(N-2));
}

//平均法数字滤波
u16 Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(1);
	}
	return temp_val/times;
}
