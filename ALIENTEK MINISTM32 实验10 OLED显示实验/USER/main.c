#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "oled.h"

//Mini STM32�����巶������9
//OLED��ʾ ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com	
   

 int main(void)
 {	
 	u8 t=0;	
 
	delay_init();	     //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
 	LED_Init();
   OLED_Init();			//��ʼ��OLED     
  OLED_ShowString(0,0,"ALIENTEK",24);  
	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
 	OLED_ShowString(0,40,"ATOM 2014/3/7",12);  
 	OLED_ShowString(0,52,"ASCII:",12);  
 	OLED_ShowString(64,52,"CODE:",12);  
	OLED_Refresh_Gram();//������ʾ��OLED	 
	t=' ';  
	while(1) 
	{		
		OLED_ShowChar(36,52,t,12,1);//��ʾASCII�ַ�	
		OLED_ShowNum(94,52,t,3,12);	//��ʾASCII�ַ�����ֵ    
		OLED_Refresh_Gram();//������ʾ��OLED
		t++;
		if(t>'~')t=' ';  
		delay_ms(500);
		LED0=!LED0;
	}	  
}


