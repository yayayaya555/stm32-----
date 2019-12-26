#include "timer5_Cap.h"

u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态
u16	TIM5CH1_CAPTURE_VAL;	//输入捕获值

//定时器5通道1输入捕获配置
void TIM5_Cap_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);   //使能TIM5时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;    //PA0 清除之前设置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PA0 下拉

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;    //PA1 清除之前设置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA1 输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA1 下拉

    //初始化定时器5 TIM5
    TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM5输入捕获参数,通道1
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    //初始化TIM5输入捕获参数,通道2
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC2映射到TI2上
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI2上
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    //中断分组初始化
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC1IE捕获中断

    TIM_Cmd(TIM5,ENABLE ); 	//使能定时器5
}

//定时器5中断服务程序
void TIM5_IRQHandler(void)
{
    if((TIM5CH1_CAPTURE_STA&0X80)==0)//还未成功捕获
    {
        if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
        {
            if(TIM5CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
            {
                if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
                {
                    TIM5CH1_CAPTURE_STA|=0X80;//标记成功捕获了一次
                    TIM5CH1_CAPTURE_VAL=0XFFFF;
                } 				
				else 
					TIM5CH1_CAPTURE_STA++;
            }
        }
        if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
        {
			if((TIM5CH1_CAPTURE_STA&0X40)==0)
			{
				TIM5CH1_CAPTURE_STA=0;			//清空
				TIM5CH1_CAPTURE_VAL=0;
				TIM_SetCounter(TIM5,0);
				TIM5CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿			
			}

        }
        if (TIM_GetITStatus(TIM5, TIM_IT_CC2) != RESET)//捕获2发生捕获事件
        {
            if(TIM5CH1_CAPTURE_STA&0X40)
            {
                TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次两个脉冲时间差
                TIM5CH1_CAPTURE_VAL=TIM_GetCapture2(TIM5);//通道2信号来时的计时器值
            }
        }			
    }
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update | TIM_IT_CC2); //清除中断标志位

}
