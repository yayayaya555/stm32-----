#include "timer8.h"
extern u16 BAT_PWM_num;

void TIM8_PWM_Init(u16 arr,u16 psc)//arr：自动重装值 psc：时钟预分频数
{  

	GPIO_InitTypeDef GPIO_InitStructure;//配置GPIO
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//配置定时器
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;//配置死区
	NVIC_InitTypeDef NVIC_InitStructure;//配置中断
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_TIM8, ENABLE); //使能时钟
	                                                                     	
	//定义PWM输出引脚,TIM8-CH1->PC6,TIM8-CH1N->PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  //定义为复用推挽输出，接第一路PWM
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	GPIO_SetBits(GPIOC,GPIO_Pin_6);             //置高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;  //定义为复用推挽输出，接第二路PWM
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	//初始化定时器TIM8
	TIM_TimeBaseStructure.TIM_Period = arr;           //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc;         //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //对于高级定时器TIM1、TIM8必须设置此值，否则每次的定时时间将随机
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);   //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ARRPreloadConfig(TIM8, ENABLE);               //使能TIMx在ARR上的预装载寄存器
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	//互补输出管脚PWM波形的占空比和模式设置
	TIM_OCInitStructure.TIM_Pulse = 600;                //初始占空比设定，以后改变CCR就行了	
	//上管
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //(上管)使能输出比较状态
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //(上管)输出比较极性低，反向
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;      //(上管)当MOE=0时，设置TIM1输出比较空闲状态
	//下管
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //(下管)使能输出比较状态
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      //(下管)输出比较极性低，反向    
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   //(下管)当MOE=0时，设置TIM1输出比较空闲状态 

	//PWM的通道1，类似的设置通道2,3
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);                        //第一对(上下管)引脚  
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);               //定时器1的CH1预装载使能	

	//互补的上下管刹车死区等设置
	TIM_BDTRInitStructure.TIM_OSSRState =TIM_OSSRState_Enable;//运行模式下输出
	TIM_BDTRInitStructure.TIM_OSSIState =TIM_OSSIState_Enable;//空闲模式下输出
	TIM_BDTRInitStructure.TIM_LOCKLevel =TIM_LOCKLevel_OFF;   //锁定级别,防止软件错误而提供写保护
	TIM_BDTRInitStructure.TIM_DeadTime =0x1f;	              //调整死区时间0-0xff
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;	  //刹车使能关闭
    //TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;   //刹车输入管脚极性低(没有刹车功能所以这句不用)        //刹车输出极性，即刹车控制引脚PB12接GND时，PWM停止输出  
	TIM_BDTRInitStructure.TIM_AutomaticOutput =TIM_AutomaticOutput_Enable; //自动输出功能使能
	TIM_BDTRConfig(TIM8,&TIM_BDTRInitStructure);		
		
	//需要在主函数设置中断优先级分组	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;         //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //抢占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE 主输出使能
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update); //清除更新中断请求位
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);    //开启定时器中断
					
	TIM_Cmd(TIM8, ENABLE);  //使能TIM1
    
}

void TIM8_UP_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)   //检查指定的TIM中断发生与否:TIM 中断源 
	{		
		TIM_SetCompare1(TIM8,BAT_PWM_num);
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);      //手动清除TIMx的中断待处理位:TIM 中断源 
	}	
}


void TIM8_CC0_Init(void)//电池待机
{
	GPIO_InitTypeDef GPIO_InitStructure;//配置GPIO
	TIM_Cmd(TIM8, DISABLE);  //除能TIM8
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  //定义为推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);             //置高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;  //定义为推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOA,GPIO_Pin_7);             //置高	
}

void TIM8_CC1_Init(void)//电池工作
{
	GPIO_InitTypeDef GPIO_InitStructure;//配置GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;  //定义为复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;  //定义为复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	TIM_Cmd(TIM8, ENABLE);  //使能TIM8
}

