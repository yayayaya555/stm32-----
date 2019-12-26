#include "pwm.h"
#include "adc.h"

u16 arr,psc=0;//arr���Զ���װֵ
			  //psc��ʱ��Ԥ��Ƶ��
float clock_PWM=72000;//��λKHz

//u8 Current_num=0;//<0-3>ԭʼAD����Ƶ��Ϊ20Khz�������Ĵ�ȡƽ��ֵ����ʵ��5Khz��һ������100����


extern float M;
//��ѡ�ø߼���ʱ��TIM1��TIM8ʵ�ֻ������
//����PWMƵ��(Khz)=72000/((arr+1)*(psc+1))�����ڼ������������=����PWMƵ��/���Ʋ�Ƶ��  
//ʵ��PWMƵ��HzΪ����PWMƵ�ʣ�2*Hz����һ�룬��Ϊ���õ���������뷽ʽ

//���磺����ʵ��ϣ���ز�Ƶ��Ϊ10Khz��������PWMƵ��Ϊ20KHz�������arrֵΪ72000/10/2=3600
//      Ȼ��ϣ�����Ʋ�Ƶ��50Hz�������Ӧȡ20K/50=400���㣬��ֵѡȡarrֵ����
//      PWMƵ��20KHz����һ���жϣ���������������뷽ʽ��ʹ�����չ۲⵽���ز�Ƶ��ֻ��10KHz
void TIM1_PWM_Init(float Carrier_Hz)
{  

	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//���ö�ʱ��
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;//��������
	NVIC_InitTypeDef NVIC_InitStructure;//�����ж�
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|
						   RCC_APB2Periph_AFIO  | RCC_APB2Periph_TIM1, ENABLE); //ʹ��ʱ��
	                                                                     	
	//����PWM�������(A8<->B13)�����ƵĶ����������ԣ�(A9<->B14)(A10<->B15)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	arr=(u16)(clock_PWM/Carrier_Hz/2)-1;//������װֵ������������뷽ʽ����2
	//��ʼ����ʱ��TIM1
	TIM_TimeBaseStructure.TIM_Period = arr;           //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc;         //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //���ڸ߼���ʱ��TIM1��TIM8�������ô�ֵ������ÿ�εĶ�ʱʱ�佫���
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;//TIM����������ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);   //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ARRPreloadConfig(TIM1, ENABLE);               //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	//��������ܽ�PWM���ε�ռ�ձȺ�ģʽ����
	TIM_OCInitStructure.TIM_Pulse = 0;                //���ֵ��ϵ����λ���⣬��ʼ����ʱ���0���Ժ����ı�CCR������	
	//�Ϲ�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //(�Ϲ�)ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //(�Ϲ�)����Ƚϼ��Եͣ�����
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;      //(�Ϲ�)��MOE=0ʱ������TIM1����ȽϿ���״̬
	//�¹�
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //(�¹�)ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      //(�¹�)����Ƚϼ��Եͣ�����    
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   //(�¹�)��MOE=0ʱ������TIM1����ȽϿ���״̬ 
	
	//PWM1��ͨ��1�����Ƶ�����ͨ��2,3
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);                        //��һ��(���¹�)����  
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);               //��ʱ��1��CH1Ԥװ��ʹ��	

	//���������¹�ɲ������������
	TIM_BDTRInitStructure.TIM_OSSRState =TIM_OSSRState_Enable;//����ģʽ�����
	TIM_BDTRInitStructure.TIM_OSSIState =TIM_OSSIState_Enable;//����ģʽ�����
	TIM_BDTRInitStructure.TIM_LOCKLevel =TIM_LOCKLevel_OFF;   //��������,��ֹ���������ṩд����
	TIM_BDTRInitStructure.TIM_DeadTime =0x1f;	              //��������ʱ��0-0xff
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;	  //ɲ��ʹ�ܹر�
    //TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;   //ɲ������ܽż��Ե�(û��ɲ������������䲻��)        //ɲ��������ԣ���ɲ����������PB12��GNDʱ��PWMֹͣ���  
	TIM_BDTRInitStructure.TIM_AutomaticOutput =TIM_AutomaticOutput_Enable; //�Զ��������ʹ��
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);		
		
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);           //������ʱ���ж�
	//��Ҫ�������������ж����ȼ�����	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;         //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��		

    
}

void TIM1_UP_IRQHandler(void)
{	
	static 	u16 pwmvalA=0;//PWM1��������
	u16 levelA;           //PWM1��װֵ
	//SPWM����ȡ������ֵѡ��arrֵ�����������ƶ�ѡ1������������PWMƵ�ʣ�72000/arr,Ϊʵ��PWMƵ�ʵ�2����	
	static float SPWM[400]=
	{
		0.0000,0.0157,0.0314,0.0471,0.0628,0.0785,0.0941,0.1097,0.1253,0.1409,0.1564,0.1719,0.1874,0.2028,0.2181,0.2334,0.2487,0.2639,0.2790,0.2940,
		0.3090,0.3239,0.3387,0.3535,0.3681,0.3827,0.3971,0.4115,0.4258,0.4399,0.4540,0.4679,0.4818,0.4955,0.5090,0.5225,0.5358,0.5490,0.5621,0.5750,
		0.5878,0.6004,0.6129,0.6252,0.6374,0.6494,0.6613,0.6730,0.6845,0.6959,0.7071,0.7181,0.7290,0.7396,0.7501,0.7604,0.7705,0.7804,0.7902,0.7997,
		0.8090,0.8181,0.8271,0.8358,0.8443,0.8526,0.8607,0.8686,0.8763,0.8838,0.8910,0.8980,0.9048,0.9114,0.9178,0.9239,0.9298,0.9354,0.9409,0.9461,
		0.9511,0.9558,0.9603,0.9646,0.9686,0.9724,0.9759,0.9792,0.9823,0.9851,0.9877,0.9900,0.9921,0.9940,0.9956,0.9969,0.9980,0.9989,0.9995,0.9999,
		1.0000,0.9999,0.9995,0.9989,0.9980,0.9969,0.9956,0.9940,0.9921,0.9900,0.9877,0.9851,0.9823,0.9792,0.9759,0.9724,0.9686,0.9646,0.9603,0.9558,
		0.9511,0.9461,0.9409,0.9354,0.9298,0.9239,0.9178,0.9114,0.9048,0.8980,0.8910,0.8838,0.8763,0.8686,0.8607,0.8526,0.8443,0.8358,0.8271,0.8181,
		0.8090,0.7997,0.7902,0.7804,0.7705,0.7604,0.7501,0.7396,0.7290,0.7181,0.7071,0.6959,0.6845,0.6730,0.6613,0.6494,0.6374,0.6252,0.6129,0.6004,
		0.5878,0.5750,0.5621,0.5490,0.5358,0.5225,0.5090,0.4955,0.4818,0.4679,0.4540,0.4399,0.4258,0.4115,0.3971,0.3827,0.3681,0.3535,0.3387,0.3239,
		0.3090,0.2940,0.2790,0.2639,0.2487,0.2334,0.2181,0.2028,0.1874,0.1719,0.1564,0.1409,0.1253,0.1097,0.0941,0.0785,0.0628,0.0471,0.0314,0.0157,
		0.0000,-0.0157,-0.0314,-0.0471,-0.0628,-0.0785,-0.0941,-0.1097,-0.1253,-0.1409,-0.1564,-0.1719,-0.1874,-0.2028,-0.2181,-0.2334,-0.2487,-0.2639,-0.2790,-0.2940,
		-0.3090,-0.3239,-0.3387,-0.3535,-0.3681,-0.3827,-0.3971,-0.4115,-0.4258,-0.4399,-0.4540,-0.4679,-0.4818,-0.4955,-0.5090,-0.5225,-0.5358,-0.5490,-0.5621,-0.5750,
		-0.5878,-0.6004,-0.6129,-0.6252,-0.6374,-0.6494,-0.6613,-0.6730,-0.6845,-0.6959,-0.7071,-0.7181,-0.7290,-0.7396,-0.7501,-0.7604,-0.7705,-0.7804,-0.7902,-0.7997,
		-0.8090,-0.8181,-0.8271,-0.8358,-0.8443,-0.8526,-0.8607,-0.8686,-0.8763,-0.8838,-0.8910,-0.8980,-0.9048,-0.9114,-0.9178,-0.9239,-0.9298,-0.9354,-0.9409,-0.9461,
		-0.9511,-0.9558,-0.9603,-0.9646,-0.9686,-0.9724,-0.9759,-0.9792,-0.9823,-0.9851,-0.9877,-0.9900,-0.9921,-0.9940,-0.9956,-0.9969,-0.9980,-0.9989,-0.9995,-0.9999,
		-1.0000,-0.9999,-0.9995,-0.9989,-0.9980,-0.9969,-0.9956,-0.9940,-0.9921,-0.9900,-0.9877,-0.9851,-0.9823,-0.9792,-0.9759,-0.9724,-0.9686,-0.9646,-0.9603,-0.9558,
		-0.9511,-0.9461,-0.9409,-0.9354,-0.9298,-0.9239,-0.9178,-0.9114,-0.9048,-0.8980,-0.8910,-0.8838,-0.8763,-0.8686,-0.8607,-0.8526,-0.8443,-0.8358,-0.8271,-0.8181,
		-0.8090,-0.7997,-0.7902,-0.7804,-0.7705,-0.7604,-0.7501,-0.7396,-0.7290,-0.7181,-0.7071,-0.6959,-0.6845,-0.6730,-0.6613,-0.6494,-0.6374,-0.6252,-0.6129,-0.6004,
		-0.5878,-0.5750,-0.5621,-0.5490,-0.5358,-0.5225,-0.5090,-0.4955,-0.4818,-0.4679,-0.4540,-0.4399,-0.4258,-0.4115,-0.3971,-0.3827,-0.3681,-0.3535,-0.3387,-0.3239,
		-0.3090,-0.2940,-0.2790,-0.2639,-0.2487,-0.2334,-0.2181,-0.2028,-0.1874,-0.1719,-0.1564,-0.1409,-0.1253,-0.1097,-0.0941,-0.0785,-0.0628,-0.0471,-0.0314,-0.0157				
	};
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)   //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{		
		levelA=(u16)((SPWM[pwmvalA++]*arr*M+arr)/2+0.5);//������װֵ
		if(pwmvalA==400)
			pwmvalA=0;
		TIM1->CCR1 = levelA;                            //��װTIM1_1��ֵ
		//TIM_SetCompare1(TIM1,levelA);
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);      //�ֶ����TIMx���жϴ�����λ:TIM �ж�Դ 
	}	
}	
