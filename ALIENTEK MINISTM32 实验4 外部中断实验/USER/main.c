#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "exti.h" 
//ALIENTEK Mini STM32开发板范例代码4
//外部中断实验   
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 int main(void)
 {	
 
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(9600);	 //串口初始化为9600
	LED_Init();		  	 //初始化与LED连接的硬件接口 
 	EXTIX_Init();		//外部中断初始化
  LED0=0;				//点亮LED
	while(1)
	{	    
		printf("OK\n");	
		delay_ms(1000);	  
	} 
}


