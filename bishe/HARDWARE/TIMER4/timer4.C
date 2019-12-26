#include "timer4.h"

extern u16 PV_Boost_num;

void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //TIM4时钟使能，TIM4挂在APB1上
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//GPIOB时钟使能
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //1->TIM2-7的时钟为APB1，0->TIM2-7的时钟为APB2
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	//定义TIM4_CH1输出引脚(PB6)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_Pulse = arr/2;            //占空比	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //使能输出比较状态
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //PWM1-Low，PWM2-High
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;    //当MOE=0时，设置TIM1输出比较空闲状态
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);                    //初始化TIM4_CH1

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                 //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);        //开启定时器4中断
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);//开启定时器4的CH1预装载使能	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM5外设							 
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{			
		TIM4->CCR1 = PV_Boost_num;                            //重装TIM1_1的值
		//TIM_SetCompare1(TIM4,PV_Boost_num);
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);    //手动清除TIMx的中断待处理位:TIM 中断源 
	}
}

