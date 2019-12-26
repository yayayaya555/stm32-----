#include "timer2.h"
#include "adc.h"

extern u16 AC_Voltage_Average,AC_Current_Average,Grid_Voltage_Average;
extern u16 PV_Voltage_Average,PV_Current_Average;
extern u16 BAT_Voltage_Average,BAT_Current_Average,Bus_Voltage_Average;
extern u16 Load_Current_Average;

u16 num=0,Display_num=0;

u32 AC_Voltage_Sample=0,AC_Current_Sample=0,Grid_Voltage_Sample=0;
u32 PV_Voltage_Sample=0,PV_Current_Sample=0;
u32 BAT_Voltage_Sample=0,BAT_Current_Sample=0,Bus_Voltage_Sample=0;
u32 Load_Current_Sample=0;

extern u16 Display_flag;
extern u16 Adjust_flag;

void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2ʱ��ʹ�ܣ�TIM2����APB1��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//GPIOAʱ��ʹ��
	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //1->TIM2-7��ʱ��ΪAPB1��0->TIM2-7��ʱ��ΪAPB2
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	//����TIM2_CH2�������(A1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_Pulse = arr/2;            //ռ�ձ�	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //PWM1-Low��PWM2-High
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;    //��MOE=0ʱ������TIM1����ȽϿ���״̬
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);                    

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //ADC1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                 //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);        //������ʱ��2�ж�
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);//������ʱ��2��CH2Ԥװ��ʹ��	
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2����							 
}

void TIM2_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)   //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{			
		if(num<100)//ÿ50ms
		{
			AC_Voltage_Sample+=Get_Adc(ADC_Channel_4);//���������ѹ��Чֵ
			AC_Current_Sample+=Get_Adc(ADC_Channel_5);//�������������Чֵ
			Grid_Voltage_Sample+=Get_Adc(ADC_Channel_6);//����������ѹ��Чֵ
			
			PV_Voltage_Sample+=Get_Adc(ADC_Channel_8);//���������ص�ѹƽ��ֵ
			PV_Current_Sample+=Get_Adc(ADC_Channel_9);//���������ص���ƽ��ֵ
			
			BAT_Voltage_Sample+=Get_Adc(ADC_Channel_10);//������ص�ѹƽ��ֵ
			BAT_Current_Sample+=Get_Adc(ADC_Channel_11);//������ص���ƽ��ֵ
			Bus_Voltage_Sample+=Get_Adc(ADC_Channel_12);//����ĸ�ߵ�ѹƽ��ֵ
			
			Load_Current_Sample+=Get_Adc(ADC_Channel_3);//�������ص���ƽ��ֵ
            
			num++;
			if(num==100)	
			{
				AC_Voltage_Average=AC_Voltage_Sample/num;
				AC_Current_Average=AC_Current_Sample/num;
				Grid_Voltage_Average=Grid_Voltage_Sample/num;
				
				PV_Voltage_Average=PV_Voltage_Sample/num;
				PV_Current_Average=PV_Current_Sample/num;
				
				BAT_Voltage_Average=BAT_Voltage_Sample/num;
				BAT_Current_Average=BAT_Current_Sample/num;
				Bus_Voltage_Average=Bus_Voltage_Sample/num;	

				Load_Current_Average=Load_Current_Sample/num;				
				
				num=0;                                //����ϵ�мĴ���
				AC_Voltage_Sample=0;
				AC_Current_Sample=0;
				Grid_Voltage_Sample=0;
				
				PV_Voltage_Sample=0;
				PV_Current_Sample=0;
				
				BAT_Voltage_Sample=0;
				BAT_Current_Sample=0;
				Bus_Voltage_Sample=0;
				
				Load_Current_Sample=0;
				
				Adjust_flag=1;	
			}				
	    }
		
		Display_num++;
		if(Display_num==1000)
		{
			Display_num=0;
			Display_flag=1;
		}
				
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);    //�ֶ����TIMx���жϴ�����λ:TIM �ж�Դ 
	}

}

