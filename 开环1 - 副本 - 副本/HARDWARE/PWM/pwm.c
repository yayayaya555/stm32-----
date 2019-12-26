#include "pwm.h"
#include "adc.h"

u16 arr,psc=0;//arr：自动重装值
			  //psc：时钟预分频数
float clock_PWM=72000;//单位KHz

//u8 Current_num=0;//<0-3>原始AD采样频率为20Khz，采样四次取平均值，即实际5Khz，一个周期100个点


extern float M;
//可选用高级定时器TIM1，TIM8实现互补输出
//理论PWM频率(Khz)=72000/((arr+1)*(psc+1))，用于计算点数，点数=理论PWM频率/调制波频率  
//实际PWM频率Hz为理论PWM频率（2*Hz）的一半，因为采用的是中央对齐方式

//例如：本次实际希望载波频率为10Khz，则理论PWM频率为20KHz，计算的arr值为72000/10/2=3600
//      然后，希望调制波频率50Hz，则点数应取20K/50=400个点，幅值选取arr值即可
//      PWM频率20KHz触发一次中断，但由于是中央对齐方式，使得最终观测到的载波频率只有10KHz
void TIM1_PWM_Init(float Carrier_Hz)
{  

	GPIO_InitTypeDef GPIO_InitStructure;//配置GPIO
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//配置定时器
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;//配置死区
	NVIC_InitTypeDef NVIC_InitStructure;//配置中断
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|
						   RCC_APB2Periph_AFIO  | RCC_APB2Periph_TIM1, ENABLE); //使能时钟
	                                                                     	
	//定义PWM输出引脚(A8<->B13)，类似的定义另外两对，(A9<->B14)(A10<->B15)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	arr=(u16)(clock_PWM/Carrier_Hz/2)-1;//计算重装值，采用中央对齐方式除以2
	//初始化定时器TIM1
	TIM_TimeBaseStructure.TIM_Period = arr;           //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc;         //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //对于高级定时器TIM1、TIM8必须设置此值，否则每次的定时时间将随机
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;//TIM中央对齐计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);   //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ARRPreloadConfig(TIM1, ENABLE);               //使能TIMx在ARR上的预装载寄存器
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	//互补输出管脚PWM波形的占空比和模式设置
	TIM_OCInitStructure.TIM_Pulse = 0;                //这个值关系到相位问题，初始化的时候给0，以后程序改变CCR就行了	
	//上管
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //(上管)使能输出比较状态
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //(上管)输出比较极性低，反向
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;      //(上管)当MOE=0时，设置TIM1输出比较空闲状态
	//下管
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //(下管)使能输出比较状态
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      //(下管)输出比较极性低，反向    
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   //(下管)当MOE=0时，设置TIM1输出比较空闲状态 
	
	//PWM1的通道1，类似的设置通道2,3
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);                        //第一对(上下管)引脚  
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);               //定时器1的CH1预装载使能	

	//互补的上下管刹车死区等设置
	TIM_BDTRInitStructure.TIM_OSSRState =TIM_OSSRState_Enable;//运行模式下输出
	TIM_BDTRInitStructure.TIM_OSSIState =TIM_OSSIState_Enable;//空闲模式下输出
	TIM_BDTRInitStructure.TIM_LOCKLevel =TIM_LOCKLevel_OFF;   //锁定级别,防止软件错误而提供写保护
	TIM_BDTRInitStructure.TIM_DeadTime =0x1f;	              //调整死区时间0-0xff
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;	  //刹车使能关闭
    //TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;   //刹车输入管脚极性低(没有刹车功能所以这句不用)        //刹车输出极性，即刹车控制引脚PB12接GND时，PWM停止输出  
	TIM_BDTRInitStructure.TIM_AutomaticOutput =TIM_AutomaticOutput_Enable; //自动输出功能使能
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);		
		
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);           //开启定时器中断
	//需要在主函数设置中断优先级分组	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;         //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //抢占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能		

    
}

void TIM1_UP_IRQHandler(void)
{	
	static 	u16 pwmvalA=0;//PWM1计数个数
	u16 levelA;           //PWM1重装值
	//SPWM数组取法：幅值选用arr值，点数，调制度选1，点数按理论PWM频率（72000/arr,为实际PWM频率的2倍）	
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
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)   //检查指定的TIM中断发生与否:TIM 中断源 
	{		
		levelA=(u16)((SPWM[pwmvalA++]*arr*M+arr)/2+0.5);//计算重装值
		if(pwmvalA==400)
			pwmvalA=0;
		TIM1->CCR1 = levelA;                            //重装TIM1_1的值
		//TIM_SetCompare1(TIM1,levelA);
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);      //手动清除TIMx的中断待处理位:TIM 中断源 
	}	
}	
