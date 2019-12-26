#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "wkup.h"
//ALIENTEK Mini STM32¿ª·¢°å·¶Àý´úÂë14
//´ý»ú»½ÐÑÊµÑé  
//¼¼ÊõÖ§³Ö£ºwww.openedv.com
//¹ãÖÝÊÐÐÇÒíµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾
   	
 int main(void)
 { 
 
	delay_init();	    	 //ÑÓÊ±º¯Êý³õÊ¼»
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶·Ö×é2  
	uart_init(9600);	 	//´®¿Ú³õÊ¼»¯Îª9600
	LED_Init();		  		//³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼þ½Ó¿Ú
 	WKUP_Init();			//³õÊ¼»¯WK_UP°´¼ü£¬Í¬Ê±¼ì²âÊÇ·ñÕý³£¿ª»ú£¿
	LCD_Init();			   	//³õÊ¼»¯LCD 			 	
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Mini STM32");	
	LCD_ShowString(30,70,200,16,16,"WKUP TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/3/8");	 
	while(1)
	{
		LED0=!LED0;
		delay_ms(250);
	}											    
}	



