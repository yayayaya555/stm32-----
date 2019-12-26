#include "timer.h"
//#include "led.h"

//通用定时器TIM2_CH2初始化
//时钟为APB1 36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器2!
void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;//配置GPIO
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2时钟使能，TIM2挂在APB1上
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//GPIOA时钟使能
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //1->TIM2-7的时钟为APB1，0->TIM2-7的时钟为APB2
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	//定义TIM2_CH2输出引脚(A1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_Pulse = arr/4;            //占空比	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //使能输出比较状态
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //PWM1-Low，PWM2-High
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;    //当MOE=0时，设置TIM1输出比较空闲状态
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);                    
	
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);               //定时器2的CH2预装载使能	
	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设							 
}

