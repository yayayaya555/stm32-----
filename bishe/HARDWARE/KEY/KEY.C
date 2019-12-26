#include "KEY.h"
#include "sys.h" 
#include "delay.h"
#include "Timer8.h"

//extern float M;
//extern u16 PV_Boost_num;
//extern u8 Boost_Buck_flag;
//extern u16 BAT_PWM_num;

extern u8 Relay_SwitchOFF_flag,flag;
extern u8 Island_Grid_flag;
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA��Cʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);//�ر�JTAG,ʹ��SWD��������SWDģʽ����
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.5
		
}
//������������
//mode:0,��֧��������;1,֧��������;

void KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������
		
	if(key_up&&(KEY0==0||KEY1==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;

        if(KEY0==0)
		{
			if(Island_Grid_flag==1)
			{
				Relay_SwitchOFF_flag=1;//������բָ��
			}
//			BAT_PWM_num+=10;		
//			if(M<0.96)
//				M+=0.02;
//			Boost_Buck_flag=1;//Boost
//			TIM8_CC1_Init();
//			Relay_SwitchOFF_flag=1;
//			Relay_SwitchON_flag=0;
		}
		else if(KEY1==0)
		{
			if(Island_Grid_flag==0)
			{
				Island_Grid_flag=1;//�µ�����
				flag=0;
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);//������բ
				TIM8_CC1_Init();//��ع���
			}
//			BAT_PWM_num-=10;
//			if(M>0.7)
//			 M-=0.02;
//			Boost_Buck_flag=2;//Buck
//			TIM8_CC2_Init();
//			Relay_SwitchON_flag=1;
//			Relay_SwitchOFF_flag=0;
		}
	
	}
	else if(KEY0==1&&KEY1==1)
		key_up=1;// �ް������� 	    
 	
}

