 #include "adc.h"
 #include "delay.h"
 #include "usart.h"
 
//��ʼ��ADC
//�Թ���ͨ��Ϊ��

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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12M,ADC���ʱ�䲻�ܳ���14M

	//����ģ��ͨ����������
	//<PA0-ADC_Channel_0>,<PA1-ADC_Channel_1>,<PA2-ADC_Channel_2>,<PA3-ADC_Channel_3>                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ,����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//ת����TIM2_CH2�ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת��ʱ��ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;  //ADC1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
  		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);//ת��ʱ��
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼   
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);//����ADC1�ж� 
	ADC_ExternalTrigConvCmd(ADC1,ENABLE);    //�����ⲿ����ģʽʹ�ܣ�������ⲿ����ʵ��������//����ADCģ����ⲿ��ʵ���ϻ�����STM32�ڲ�)			  
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //�������������������	

}

void ADC1_2_IRQHandler(void)
{

	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) !=RESET)//���ָ����ADC�ж��Ƿ���
	{
//		if(Current_num<4)
//		{
//			ALL_Current_Sample+=ADC_GetConversionValue(ADC1);//�������һ��ADC1�������ת�����
//			Current_num++;					
//	    }
//		if(Current_num==4)
//		{
//			Current_Average=ALL_Current_Sample/4;
//			if((Usart_Send_flag==1)&&(num<=2500))
//			{	
//				USART();
//				//printf("%d\r\n",Current_Average);	//��ӡ������
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
				//printf("%d\r\n",Current_Average);	//��ӡ������
				num++;
				if(num==2501)
				{	
					Usart_Send_flag=0;
                    num=1;
				}					
			}
		
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC); //�ֶ����ADC1���жϴ�����λ
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
		while((USART1->SR&0X40)==0);//�ȴ����ͽ���
	}
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

//ð�ݷ����������˲�
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

//ƽ���������˲�
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
