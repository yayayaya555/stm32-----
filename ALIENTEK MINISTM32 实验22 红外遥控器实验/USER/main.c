#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "remote.h"   
//ALIENTEK Mini STM32¿ª·¢°å·¶Àı´úÂë22
//ºìÍâÒ£¿ØÊµÑé  
//¼¼ÊõÖ§³Ö£ºwww.openedv.com
//¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 

 int main(void)
 { 
	u8 key;
	u8 t=0;	
	u8 *str=0; 	
	delay_init();	    	 //ÑÓÊ±º¯Êı³õÊ¼»
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// ÉèÖÃÖĞ¶ÏÓÅÏÈ¼¶·Ö×é2	  
	uart_init(9600);	 	//´®¿Ú³õÊ¼»¯Îª9600
	LED_Init();		  		//³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼ş½Ó¿Ú
 	LCD_Init();
	Remote_Init();			//ºìÍâ½ÓÊÕ³õÊ¼»¯	 
 	POINT_COLOR=RED;//ÉèÖÃ×ÖÌåÎªºìÉ« 
	LCD_ShowString(60,50,200,16,16,"Mini STM32");	
	LCD_ShowString(60,70,200,16,16,"REMOTE TEST");	
	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,"2014/3/12");

   	LCD_ShowString(60,130,200,16,16,"KEYVAL:");	
   	LCD_ShowString(60,150,200,16,16,"KEYCNT:");	
   	LCD_ShowString(60,170,200,16,16,"SYMBOL:");	  		 	  		    							  
	while(1)
	{
		key=Remote_Scan();	
		if(key)
		{	 
			LCD_ShowNum(116,130,key,3,16);		//ÏÔÊ¾¼üÖµ
			LCD_ShowNum(116,150,RmtCnt,3,16);	//ÏÔÊ¾°´¼ü´ÎÊı		  
			switch(key)
			{
				case 0:str="ERROR";break;			   
				case 162:str="POWER";break;	    
				case 98:str="UP";break;	    
				case 2:str="PLAY";break;		 
				case 226:str="ALIENTEK";break;		  
				case 194:str="RIGHT";break;	   
				case 34:str="LEFT";break;		  
				case 224:str="VOL-";break;		  
				case 168:str="DOWN";break;		   
				case 144:str="VOL+";break;		    
				case 104:str="1";break;		  
				case 152:str="2";break;	   
				case 176:str="3";break;	    
				case 48:str="4";break;		    
				case 24:str="5";break;		    
				case 122:str="6";break;		  
				case 16:str="7";break;			   					
				case 56:str="8";break;	 
				case 90:str="9";break;
				case 66:str="0";break;
				case 82:str="DELETE";break;		 
			}
			LCD_Fill(116,170,116+8*8,170+16,WHITE);	//Çå³şÖ®Ç°µÄÏÔÊ¾
			LCD_ShowString(116,170,200,16,16,str);	//ÏÔÊ¾SYMBOL
		}else delay_ms(10);	  
		t++;
		if(t==20)
		{
			t=0;
			LED0=!LED0;
		}
	}
}




















