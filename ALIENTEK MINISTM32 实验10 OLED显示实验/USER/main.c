#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "oled.h"

//Mini STM32开发板范例代码9
//OLED显示 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com	
   

 int main(void)
 {	
 	u8 t=0;	
 
	delay_init();	     //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
 	LED_Init();
   OLED_Init();			//初始化OLED     
  OLED_ShowString(0,0,"ALIENTEK",24);  
	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
 	OLED_ShowString(0,40,"ATOM 2014/3/7",12);  
 	OLED_ShowString(0,52,"ASCII:",12);  
 	OLED_ShowString(64,52,"CODE:",12);  
	OLED_Refresh_Gram();//更新显示到OLED	 
	t=' ';  
	while(1) 
	{		
		OLED_ShowChar(36,52,t,12,1);//显示ASCII字符	
		OLED_ShowNum(94,52,t,3,12);	//显示ASCII字符的码值    
		OLED_Refresh_Gram();//更新显示到OLED
		t++;
		if(t>'~')t=' ';  
		delay_ms(500);
		LED0=!LED0;
	}	  
}


