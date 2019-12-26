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
	GPIO_InitTypeDef GPIO_InitStructure;//配置GPIO
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
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
	TIM_OCInitStructure.TIM_Pulse = arr/2;            //占空比	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //使能输出比较状态
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //PWM1-Low，PWM2-High
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;    //当MOE=0时，设置TIM1输出比较空闲状态
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);                    

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //ADC1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                 //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);        //开启定时器2中断
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);//开启定时器2的CH2预装载使能	
	TIM_Cmd(TIM2, ENABLE);  //使能TIM2外设							 
}

void TIM2_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)   //检查指定的TIM中断发生与否:TIM 中断源 
	{			
		if(num<100)//每50ms
		{
			AC_Voltage_Sample+=Get_Adc(ADC_Channel_4);//采样逆变侧电压有效值
			AC_Current_Sample+=Get_Adc(ADC_Channel_5);//采样逆变侧电流有效值
			Grid_Voltage_Sample+=Get_Adc(ADC_Channel_6);//采样电网电压有效值
			
			PV_Voltage_Sample+=Get_Adc(ADC_Channel_8);//采样光伏电池电压平均值
			PV_Current_Sample+=Get_Adc(ADC_Channel_9);//采样光伏电池电流平均值
			
			BAT_Voltage_Sample+=Get_Adc(ADC_Channel_10);//采样电池电压平均值
			BAT_Current_Sample+=Get_Adc(ADC_Channel_11);//采样电池电流平均值
			Bus_Voltage_Sample+=Get_Adc(ADC_Channel_12);//采样母线电压平均值
			
			Load_Current_Sample+=Get_Adc(ADC_Channel_3);//采样负载电流平均值
            
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
				
				num=0;                                //清零系列寄存器
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
				
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);    //手动清除TIMx的中断待处理位:TIM 中断源 
	}

}

