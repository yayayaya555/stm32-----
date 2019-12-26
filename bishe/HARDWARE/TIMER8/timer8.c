#include "timer8.h"
extern u16 BAT_PWM_num;

void TIM8_PWM_Init(u16 arr,u16 psc)//arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
{  

	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//���ö�ʱ��
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;//��������
	NVIC_InitTypeDef NVIC_InitStructure;//�����ж�
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_TIM8, ENABLE); //ʹ��ʱ��
	                                                                     	
	//����PWM�������,TIM8-CH1->PC6,TIM8-CH1N->PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  //����Ϊ��������������ӵ�һ·PWM
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	GPIO_SetBits(GPIOC,GPIO_Pin_6);             //�ø�

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;  //����Ϊ��������������ӵڶ�·PWM
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	//��ʼ����ʱ��TIM8
	TIM_TimeBaseStructure.TIM_Period = arr;           //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc;         //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //���ڸ߼���ʱ��TIM1��TIM8�������ô�ֵ������ÿ�εĶ�ʱʱ�佫���
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);   //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ARRPreloadConfig(TIM8, ENABLE);               //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	//��������ܽ�PWM���ε�ռ�ձȺ�ģʽ����
	TIM_OCInitStructure.TIM_Pulse = 600;                //��ʼռ�ձ��趨���Ժ�ı�CCR������	
	//�Ϲ�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //(�Ϲ�)ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //(�Ϲ�)����Ƚϼ��Եͣ�����
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;      //(�Ϲ�)��MOE=0ʱ������TIM1����ȽϿ���״̬
	//�¹�
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //(�¹�)ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      //(�¹�)����Ƚϼ��Եͣ�����    
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   //(�¹�)��MOE=0ʱ������TIM1����ȽϿ���״̬ 

	//PWM��ͨ��1�����Ƶ�����ͨ��2,3
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);                        //��һ��(���¹�)����  
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);               //��ʱ��1��CH1Ԥװ��ʹ��	

	//���������¹�ɲ������������
	TIM_BDTRInitStructure.TIM_OSSRState =TIM_OSSRState_Enable;//����ģʽ�����
	TIM_BDTRInitStructure.TIM_OSSIState =TIM_OSSIState_Enable;//����ģʽ�����
	TIM_BDTRInitStructure.TIM_LOCKLevel =TIM_LOCKLevel_OFF;   //��������,��ֹ���������ṩд����
	TIM_BDTRInitStructure.TIM_DeadTime =0x1f;	              //��������ʱ��0-0xff
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;	  //ɲ��ʹ�ܹر�
    //TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;   //ɲ������ܽż��Ե�(û��ɲ������������䲻��)        //ɲ��������ԣ���ɲ����������PB12��GNDʱ��PWMֹͣ���  
	TIM_BDTRInitStructure.TIM_AutomaticOutput =TIM_AutomaticOutput_Enable; //�Զ��������ʹ��
	TIM_BDTRConfig(TIM8,&TIM_BDTRInitStructure);		
		
	//��Ҫ�������������ж����ȼ�����	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;         //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE �����ʹ��
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update); //��������ж�����λ
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);    //������ʱ���ж�
					
	TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM1
    
}

void TIM8_UP_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)   //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{		
		TIM_SetCompare1(TIM8,BAT_PWM_num);
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);      //�ֶ����TIMx���жϴ�����λ:TIM �ж�Դ 
	}	
}


void TIM8_CC0_Init(void)//��ش���
{
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO
	TIM_Cmd(TIM8, DISABLE);  //����TIM8
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  //����Ϊ�������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);             //�ø�
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;  //����Ϊ�������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOA,GPIO_Pin_7);             //�ø�	
}

void TIM8_CC1_Init(void)//��ع���
{
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  //����Ϊ�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;  //����Ϊ�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM8
}

