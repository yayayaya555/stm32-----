#include "pwm.h"

u16 arr,half_arr,psc=0;//arr：自动重装值,half_arr为arr的一半
					   //psc：时钟预分频数
float clock_PWM=72000; //单位KHz

u16 levelA;     //PWM重装值
u16 pwmvalA=0;  //PWM计数个数

extern u32 temp;
extern u8 temp_flag;
//extern u8 Relay_SwitchOFF_flag, Relay_SwitchON_flag;

int error_PWM_num;//判断相位点数差值
u16 mid;//储存相位差值中间计算量
//u16 Switch_num=0;//储存相位差值中间计算量

//SPWM数组取法：幅值选用arr/2，点数，调制度选1，点数按理论PWM频率（72000/arr,为实际PWM频率的2倍）	
float SPWM[400]=
{
	0,28,56,84,113,141,169,197,225,253,281,309,337,365,392,420,
	447,474,502,529,556,583,609,636,662,688,714,740,766,791,817,842,
	867,891,916,940,964,988,1011,1035,1058,1080,1103,1125,1147,1169,1190,1211,
	1232,1252,1272,1292,1312,1331,1350,1368,1386,1404,1422,1439,1456,1472,1488,1504,
	1519,1534,1549,1563,1577,1590,1603,1616,1628,1640,1651,1662,1673,1683,1693,1702,
	1711,1720,1728,1736,1743,1750,1756,1762,1768,1773,1777,1782,1785,1789,1792,1794,
	1796,1798,1799,1799,1800,1799,1799,1798,1796,1794,1792,1789,1785,1782,1777,1773,
	1768,1762,1756,1750,1743,1736,1728,1720,1711,1702,1693,1683,1673,1662,1651,1640,
	1628,1616,1603,1590,1577,1563,1549,1534,1519,1504,1488,1472,1456,1439,1422,1404,
	1386,1368,1350,1331,1312,1292,1272,1252,1232,1211,1190,1169,1147,1125,1103,1080,
	1058,1035,1011,988,964,940,916,891,867,842,817,791,766,740,714,688,
	662,636,609,583,556,529,502,474,447,420,392,365,337,309,281,253,
	225,197,169,141,113,84,56,28,0,-28,-56,-84,-113,-141,-169,-197,
	-225,-253,-281,-309,-337,-365,-392,-420,-447,-474,-502,-529,-556,-583,-609,-636,
	-662,-688,-714,-740,-766,-791,-817,-842,-867,-891,-916,-940,-964,-988,-1011,-1035,
	-1058,-1080,-1103,-1125,-1147,-1169,-1190,-1211,-1232,-1252,-1272,-1292,-1312,-1331,-1350,-1368,
	-1386,-1404,-1422,-1439,-1456,-1472,-1488,-1504,-1519,-1534,-1549,-1563,-1577,-1590,-1603,-1616,
	-1628,-1640,-1651,-1662,-1673,-1683,-1693,-1702,-1711,-1720,-1728,-1736,-1743,-1750,-1756,-1762,
	-1768,-1773,-1777,-1782,-1785,-1789,-1792,-1794,-1796,-1798,-1799,-1799,-1800,-1799,-1799,-1798,
	-1796,-1794,-1792,-1789,-1785,-1782,-1777,-1773,-1768,-1762,-1756,-1750,-1743,-1736,-1728,-1720,
	-1711,-1702,-1693,-1683,-1673,-1662,-1651,-1640,-1628,-1616,-1603,-1590,-1577,-1563,-1549,-1534,
	-1519,-1504,-1488,-1472,-1456,-1439,-1422,-1404,-1386,-1368,-1350,-1331,-1312,-1292,-1272,-1252,
	-1232,-1211,-1190,-1169,-1147,-1125,-1103,-1080,-1058,-1035,-1011,-988,-964,-940,-916,-891,
	-867,-842,-817,-791,-766,-740,-714,-688,-662,-636,-609,-583,-556,-529,-502,-474,
	-447,-420,-392,-365,-337,-309,-281,-253,-225,-197,-169,-141,-113,-84,-56,-28			
};

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
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;  //继电器合闸控制输出，置高合闸，复位分闸
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		
	arr=(u16)(clock_PWM/Carrier_Hz/2)-1;//计算重装值，采用中央对齐方式除以2
	half_arr=(arr+1)/2;
	
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
	TIM_OCInitStructure.TIM_Pulse = 0;                //初始占空比设定，以后改变CCR就行了	
	//上管
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //(上管)使能输出比较状态
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //(上管)输出比较极性低，反向
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;      //(上管)当MOE=0时，设置TIM1输出比较空闲状态
	//下管
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //(下管)使能输出比较状态
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      //(下管)输出比较极性低，反向    
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   //(下管)当MOE=0时，设置TIM1输出比较空闲状态 
	
	//PWM的通道1，类似的设置通道2,3
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
		
	//需要在主函数设置中断优先级分组	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;         //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能
	
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //清除更新中断请求位
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);    //开启定时器中断
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1	
}

void TIM1_UP_IRQHandler(void)
{		
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)   //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);      //手动清除TIMx的中断待处理位:TIM 中断源 	
		
		levelA=(u16)(SPWM[pwmvalA]*M+half_arr);       //计算重装值
		TIM1->CCR1 = levelA;                            //重装TIM1_1的值
		pwmvalA++;
		if(pwmvalA>=400)
			pwmvalA=0;
				
		if(temp_flag==1)//若存在相位差，标志位置位
		{
			temp_flag=0;//复位标志位
			if((temp>0) && (temp<=10000))//逆变器电压超前电网电压，计数量减去超前量
			{
				mid=temp/50;
				if(mid>=2)
					mid=2;
				error_PWM_num=pwmvalA-mid;
				if(error_PWM_num>=0)
					pwmvalA-=mid;
				else
					pwmvalA=400+error_PWM_num;	
			}
			else if((temp>=10000) && (temp<=20000))//逆变器电压滞后电网电压，计数量加上滞后量
			{
				mid=(20000-temp)/50;
				if(mid>=2)
					mid=2;
				error_PWM_num=pwmvalA+mid;
				if(error_PWM_num>=400)
					pwmvalA=error_PWM_num-400;
				else
					pwmvalA+=mid;	
			}
		}		
	}
}	

