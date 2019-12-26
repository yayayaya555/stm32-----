#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "usmart.h"
//ALIENTEK Mini STM32开发板范例代码12
//USMART调试组件实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 
//LED状态设置函数
void led_set(u8 sta)
{
	LED1=sta;
} 
//函数参数调用测试函数
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
}  	
 int main(void)
 { 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	delay_init();	    	 //延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();				//初始化与LED连接的硬件接口
 	LCD_Init();				//初始化LCD
	usmart_dev.init(72); 	//初始化USMART				 	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Mini STM32 ^_^");	
	LCD_ShowString(30,70,200,16,16,"USMART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/3/8");	   
  	while(1) 
	{		 	  
		LED0=!LED0;					 
		delay_ms(500);	
	}											    
}	



