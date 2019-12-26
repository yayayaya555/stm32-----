/*2019��ҵ���˫��DC-AC�任������ܣ����ڿ������ݲ���*/
/*����鴴��ʵ����1122���У��������⴫��!*/

//������¼���ƶ�ֵ��������ѹ��������ʵ�������ѹ
//���õ��ƶȺͷ���������֮��ĺ�����ϵ������Ҫ����ϵ��������PI���ڲ���
//���÷�����������ʵ������ĺ�����ϵ��ȷ����Ҫ�����ѹ��Ӧ������������Ϊ��ѹ����ֵ��ͬʱ�ɸ��ݺ�����ϵ����Һ����ʾ

/*����ʹ��˵��
PA2:������ѹAD���룬��֤�����ŵ�ѹ���ø���3.3V������������

PA1:TIM2_CH2��PWM��������ڴ�����Ƶ����

PA0:key,����2500������
PC5: key0���ƶȼ������Ըı�ÿ�εĲ���ֵ
PA15:key1���ƶȼӣ����Ըı�ÿ�εĲ���ֵ

PA9:���ű�PWM������ڲ��Ѿ�������������ͬʱΪ�ߣ����ⲿ��·�����������·
PB13:���ű�PWM�������
*/

#include "delay.h"
#include "sys.h"
#include "pwm.h"
#include "KEY.h"
#include "oled.h"
#include "adc.h"
#include "usart.h"
#include "timer.h"
//#include "pid.h"	//�������Բ���ҪPI���ڲ��֣����Ե�������������PI������

float M=0.8;
u16 Current_Average=0;
u8 Usart_Send_flag=0;

 int main(void)
 {	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init();	    	//��ʱ������ʼ��
    KEY_Init();				//������ʼ��
	OLED_Init();			//OLEDҺ����Ļ��ʼ��,������STM32�Դ�Һ����Ļ
	Adc_Init();				//AD������ʼ������ʼ��adc1_2,(PA2)
//	TIM1_PWM_Init(10);		//PWM����ʵ���ز�Ƶ��10KHZ
	TIM2_Int_Init(1439,0);  //��ʱ��2�ļ�ʱƵ��=72000/3600=20KHz
	uart_init(9600);	    //���ڲ�����9600	

	OLED_ShowString(0,0,"2018",16);   //����16*16
	OLED_Show_Chinese(32,0,8);				//��
	OLED_Show_Chinese(48,0,9);			    //��
	OLED_Show_Chinese(64,0,10);				//ʵ
	OLED_Show_Chinese(80,0,11);			    //��
	OLED_Show_Chinese(96,0,12);			    //��
	
	OLED_Show_Chinese(0,16,13);				//��
	OLED_Show_Chinese(16,16,14);			//��
	OLED_Show_Chinese(32,16,15);			//��
	OLED_Show_Chinese(48,16,16);			//��
	OLED_Show_Chinese(64,16,17);			//��
	
	OLED_Show_Chinese(0,32,3);				//Ƶ
	OLED_Show_Chinese(16,32,4);			    //��
  	OLED_ShowString(56,32,"Hz",16);      
	
	OLED_Show_Chinese(0,48,5);			    //��	 
	OLED_Show_Chinese(16,48,6);				//��
	OLED_Show_Chinese(32,48,7);			    //��
	OLED_ShowString(56,48,"0.",16);   
	OLED_Refresh_Gram();//������ʾ��OLED 
   	
	while(1)
	{
		KEY_Scan(0);

		OLED_ShowNum(96,16,Current_Average,4,16);//��ʾAD����ֵ
			
		OLED_ShowNum(72,48,M*100,2,16);//��ʾ���ƶ�		
		
		OLED_Refresh_Gram();//������ʾ��OLED
//		printf("%d\r\n",Current_Average);	//��ӡ������
		delay_ms(100);

	} 

}

