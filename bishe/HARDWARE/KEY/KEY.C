#include "KEY.h"
#include "sys.h" 
#include "delay.h"
#include "Timer8.h"

//extern float M;
//extern u16 PV_Boost_num;
//extern u8 Boost_Buck_flag;
//extern u16 BAT_PWM_num;

extern u8 Relay_SwitchOFF_flag,flag;
extern u8 Island_Grid_flag;
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA、C时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);//关闭JTAG,使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.5
		
}
//按键处理函数
//mode:0,不支持连续按;1,支持连续按;

void KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按
		
	if(key_up&&(KEY0==0||KEY1==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;

        if(KEY0==0)
		{
			if(Island_Grid_flag==1)
			{
				Relay_SwitchOFF_flag=1;//发出合闸指令
			}
//			BAT_PWM_num+=10;		
//			if(M<0.96)
//				M+=0.02;
//			Boost_Buck_flag=1;//Boost
//			TIM8_CC1_Init();
//			Relay_SwitchOFF_flag=1;
//			Relay_SwitchON_flag=0;
		}
		else if(KEY1==0)
		{
			if(Island_Grid_flag==0)
			{
				Island_Grid_flag=1;//孤岛运行
				flag=0;
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);//立即分闸
				TIM8_CC1_Init();//电池工作
			}
//			BAT_PWM_num-=10;
//			if(M>0.7)
//			 M-=0.02;
//			Boost_Buck_flag=2;//Buck
//			TIM8_CC2_Init();
//			Relay_SwitchON_flag=1;
//			Relay_SwitchOFF_flag=0;
		}
	
	}
	else if(KEY0==1&&KEY1==1)
		key_up=1;// 无按键按下 	    
 	
}


