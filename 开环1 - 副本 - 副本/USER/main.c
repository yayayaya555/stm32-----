/*2019毕业设计双向DC-AC变换程序汇总，用于开环数据测试*/
/*程序归创新实验室1122所有，请勿随意传播!*/

//开环记录调制度值，反馈电压数字量，实际输出电压
//利用调制度和反馈数字量之间的函数关系，仅需要比例系数，修正PI调节参数
//利用反馈数字量和实际输出的函数关系，确定需要输出电压对应的数字量，作为电压给定值，同时可根据函数关系用于液晶显示

/*引脚使用说明
PA2:反馈电压AD输入，保证此引脚电压不得高于3.3V，以免损坏引脚

PA1:TIM2_CH2的PWM输出，用于触发定频采样

PA0:key,发送2500个数据
PC5: key0调制度减，可以改变每次的步进值
PA15:key1调制度加，可以改变每次的步进值

PA9:上桥臂PWM输出，内部已经反向（死区呈现同时为高），外部电路需增设光耦反向电路
PB13:下桥臂PWM互补输出
*/

#include "delay.h"
#include "sys.h"
#include "pwm.h"
#include "KEY.h"
#include "oled.h"
#include "adc.h"
#include "usart.h"
#include "timer.h"
//#include "pid.h"	//开环测试不需要PI调节部分，测试的数据用于修正PI参数等

float M=0.8;
u16 Current_Average=0;
u8 Usart_Send_flag=0;

 int main(void)
 {	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init();	    	//延时函数初始化
    KEY_Init();				//按键初始化
	OLED_Init();			//OLED液晶屏幕初始化,适用于STM32自带液晶屏幕
	Adc_Init();				//AD采样初始化，初始化adc1_2,(PA2)
//	TIM1_PWM_Init(10);		//PWM输入实际载波频率10KHZ
	TIM2_Int_Init(1439,0);  //定时器2的计时频率=72000/3600=20KHz
	uart_init(9600);	    //串口波特率9600	

	OLED_ShowString(0,0,"2018",16);   //汉字16*16
	OLED_Show_Chinese(32,0,8);				//创
	OLED_Show_Chinese(48,0,9);			    //新
	OLED_Show_Chinese(64,0,10);				//实
	OLED_Show_Chinese(80,0,11);			    //验
	OLED_Show_Chinese(96,0,12);			    //室
	
	OLED_Show_Chinese(0,16,13);				//采
	OLED_Show_Chinese(16,16,14);			//样
	OLED_Show_Chinese(32,16,15);			//数
	OLED_Show_Chinese(48,16,16);			//字
	OLED_Show_Chinese(64,16,17);			//量
	
	OLED_Show_Chinese(0,32,3);				//频
	OLED_Show_Chinese(16,32,4);			    //率
  	OLED_ShowString(56,32,"Hz",16);      
	
	OLED_Show_Chinese(0,48,5);			    //调	 
	OLED_Show_Chinese(16,48,6);				//制
	OLED_Show_Chinese(32,48,7);			    //度
	OLED_ShowString(56,48,"0.",16);   
	OLED_Refresh_Gram();//更新显示到OLED 
   	
	while(1)
	{
		KEY_Scan(0);

		OLED_ShowNum(96,16,Current_Average,4,16);//显示AD采样值
			
		OLED_ShowNum(72,48,M*100,2,16);//显示调制度		
		
		OLED_Refresh_Gram();//更新显示到OLED
//		printf("%d\r\n",Current_Average);	//打印数字量
		delay_ms(100);

	} 

}

