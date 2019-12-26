#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
//ALIENTEK Mini STM32�����巶������9
//���벶��ʵ��   
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾


extern u8  TIM2CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u16	TIM2CH1_CAPTURE_VAL;	//���벶��ֵ
 int main(void)
 {	
	 
	u32 temp=0; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	delay_init();	    	 //��ʱ������ʼ��	
	uart_init(9600);	 			//9600	 
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
 	TIM1_PWM_Init(899,0); 			//����Ƶ��PWMƵ��=72000/(899+1)=80Khz
 	TIM2_Cap_Init(0XFFFF,72-1);		//��1Mhz��Ƶ�ʼ��� 
   	while(1)
	{
 		delay_ms(10);
		TIM_SetCompare1(TIM1,TIM_GetCapture1(TIM1)+1);
		if(TIM_GetCapture1(TIM1)==300)TIM_SetCompare1(TIM1,0);		 
		if(TIM2CH1_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ
		{
			temp=TIM2CH1_CAPTURE_STA&0X3F;
			temp*=65536;					//���ʱ���ܺ�
			temp+=TIM2CH1_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
			printf("HIGH:%d us\r\n",temp);	//��ӡ�ܵĸߵ�ƽʱ��
 			TIM2CH1_CAPTURE_STA=0;			//������һ�β���
 		}
	}
}
