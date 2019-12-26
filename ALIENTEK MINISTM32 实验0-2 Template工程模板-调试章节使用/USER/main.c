#include "sys.h"
#include "delay.h"
#include "usart.h"



 int main(void)
 {		
 	u8 t=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为9600
	while(1){
	printf("t:%d\n",t);
	delay_ms(500);
	t++;
	}
 }
