#include "timer4.h"

extern u16 PV_Boost_num;

void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //TIM4ʱ��ʹ�ܣ�TIM4����APB1��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//GPIOBʱ��ʹ��
	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //1->TIM2-7��ʱ��ΪAPB1��0->TIM2-7��ʱ��ΪAPB2
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	//����TIM4_CH1�������(PB6)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM������ȵ���ģʽ2
	TIM_OCInitStructure.TIM_Pulse = arr/2;            //ռ�ձ�	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //PWM1-Low��PWM2-High
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;    //��MOE=0ʱ������TIM1����ȽϿ���״̬
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);                    //��ʼ��TIM4_CH1

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                 //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);        //������ʱ��4�ж�
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);//������ʱ��4��CH1Ԥװ��ʹ��	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM5����							 
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{			
		TIM4->CCR1 = PV_Boost_num;                            //��װTIM1_1��ֵ
		//TIM_SetCompare1(TIM4,PV_Boost_num);
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);    //�ֶ����TIMx���жϴ�����λ:TIM �ж�Դ 
	}
}
