#include "sys.h"
#include "delay.h"
#include "usart.h"



 int main(void)
 {		
 	u8 t=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	while(1){
	printf("t:%d\n",t);
	delay_ms(500);
	t++;
	}
 }
