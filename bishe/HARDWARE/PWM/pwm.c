#include "pwm.h"

u16 arr,half_arr,psc=0;//arr���Զ���װֵ,half_arrΪarr��һ��
					   //psc��ʱ��Ԥ��Ƶ��
float clock_PWM=72000; //��λKHz

u16 levelA;     //PWM��װֵ
u16 pwmvalA=0;  //PWM��������

extern u32 temp;
extern u8 temp_flag;
//extern u8 Relay_SwitchOFF_flag, Relay_SwitchON_flag;

int error_PWM_num;//�ж���λ������ֵ
u16 mid;//������λ��ֵ�м������
//u16 Switch_num=0;//������λ��ֵ�м������

//SPWM����ȡ������ֵѡ��arr/2�����������ƶ�ѡ1������������PWMƵ�ʣ�72000/arr,Ϊʵ��PWMƵ�ʵ�2����	
float SPWM[400]=
{
	0,28,56,84,113,141,169,197,225,253,281,309,337,365,392,420,
	447,474,502,529,556,583,609,636,662,688,714,740,766,791,817,842,
	867,891,916,940,964,988,1011,1035,1058,1080,1103,1125,1147,1169,1190,1211,
	1232,1252,1272,1292,1312,1331,1350,1368,1386,1404,1422,1439,1456,1472,1488,1504,
	1519,1534,1549,1563,1577,1590,1603,1616,1628,1640,1651,1662,1673,1683,1693,1702,
	1711,1720,1728,1736,1743,1750,1756,1762,1768,1773,1777,1782,1785,1789,1792,1794,
	1796,1798,1799,1799,1800,1799,1799,1798,1796,1794,1792,1789,1785,1782,1777,1773,
	1768,1762,1756,1750,1743,1736,1728,1720,1711,1702,1693,1683,1673,1662,1651,1640,
	1628,1616,1603,1590,1577,1563,1549,1534,1519,1504,1488,1472,1456,1439,1422,1404,
	1386,1368,1350,1331,1312,1292,1272,1252,1232,1211,1190,1169,1147,1125,1103,1080,
	1058,1035,1011,988,964,940,916,891,867,842,817,791,766,740,714,688,
	662,636,609,583,556,529,502,474,447,420,392,365,337,309,281,253,
	225,197,169,141,113,84,56,28,0,-28,-56,-84,-113,-141,-169,-197,
	-225,-253,-281,-309,-337,-365,-392,-420,-447,-474,-502,-529,-556,-583,-609,-636,
	-662,-688,-714,-740,-766,-791,-817,-842,-867,-891,-916,-940,-964,-988,-1011,-1035,
	-1058,-1080,-1103,-1125,-1147,-1169,-1190,-1211,-1232,-1252,-1272,-1292,-1312,-1331,-1350,-1368,
	-1386,-1404,-1422,-1439,-1456,-1472,-1488,-1504,-1519,-1534,-1549,-1563,-1577,-1590,-1603,-1616,
	-1628,-1640,-1651,-1662,-1673,-1683,-1693,-1702,-1711,-1720,-1728,-1736,-1743,-1750,-1756,-1762,
	-1768,-1773,-1777,-1782,-1785,-1789,-1792,-1794,-1796,-1798,-1799,-1799,-1800,-1799,-1799,-1798,
	-1796,-1794,-1792,-1789,-1785,-1782,-1777,-1773,-1768,-1762,-1756,-1750,-1743,-1736,-1728,-1720,
	-1711,-1702,-1693,-1683,-1673,-1662,-1651,-1640,-1628,-1616,-1603,-1590,-1577,-1563,-1549,-1534,
	-1519,-1504,-1488,-1472,-1456,-1439,-1422,-1404,-1386,-1368,-1350,-1331,-1312,-1292,-1272,-1252,
	-1232,-1211,-1190,-1169,-1147,-1125,-1103,-1080,-1058,-1035,-1011,-988,-964,-940,-916,-891,
	-867,-842,-817,-791,-766,-740,-714,-688,-662,-636,-609,-583,-556,-529,-502,-474,
	-447,-420,-392,-365,-337,-309,-281,-253,-225,-197,-169,-141,-113,-84,-56,-28			
};

extern float M;
//��ѡ�ø߼���ʱ��TIM1��TIM8ʵ�ֻ������
//����PWMƵ��(Khz)=72000/((arr+1)*(psc+1))�����ڼ������������=����PWMƵ��/���Ʋ�Ƶ��  
//ʵ��PWMƵ��HzΪ����PWMƵ�ʣ�2*Hz����һ�룬��Ϊ���õ���������뷽ʽ

//���磺����ʵ��ϣ���ز�Ƶ��Ϊ10Khz��������PWMƵ��Ϊ20KHz�������arrֵΪ72000/10/2=3600
//      Ȼ��ϣ�����Ʋ�Ƶ��50Hz�������Ӧȡ20K/50=400���㣬��ֵѡȡarrֵ����
//      PWMƵ��20KHz����һ���жϣ���������������뷽ʽ��ʹ�����չ۲⵽���ز�Ƶ��ֻ��10KHz
void TIM1_PWM_Init(float Carrier_Hz)
{  

	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//���ö�ʱ��
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;//��������
	NVIC_InitTypeDef NVIC_InitStructure;//�����ж�
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|
						   RCC_APB2Periph_AFIO  | RCC_APB2Periph_TIM1, ENABLE); //ʹ��ʱ��
	                                                                     	
	//����PWM�������(A8<->B13)�����ƵĶ����������ԣ�(A9<->B14)(A10<->B15)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;  //�̵�����բ����������øߺ�բ����λ��բ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		
	arr=(u16)(clock_PWM/Carrier_Hz/2)-1;//������װֵ������������뷽ʽ����2
	half_arr=(arr+1)/2;
	
	//��ʼ����ʱ��TIM1
	TIM_TimeBaseStructure.TIM_Period = arr;           //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc;         //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //���ڸ߼���ʱ��TIM1��TIM8�������ô�ֵ������ÿ�εĶ�ʱʱ�佫���
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;//TIM����������ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);   //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ARRPreloadConfig(TIM1, ENABLE);               //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	//��������ܽ�PWM���ε�ռ�ձȺ�ģʽ����
	TIM_OCInitStructure.TIM_Pulse = 0;                //��ʼռ�ձ��趨���Ժ�ı�CCR������	
	//�Ϲ�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //(�Ϲ�)ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //(�Ϲ�)����Ƚϼ��Եͣ�����
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;      //(�Ϲ�)��MOE=0ʱ������TIM1����ȽϿ���״̬
	//�¹�
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //(�¹�)ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      //(�¹�)����Ƚϼ��Եͣ�����    
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   //(�¹�)��MOE=0ʱ������TIM1����ȽϿ���״̬ 
	
	//PWM��ͨ��1�����Ƶ�����ͨ��2,3
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);                        //��һ��(���¹�)����  
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);               //��ʱ��1��CH1Ԥװ��ʹ��	

	//���������¹�ɲ������������
	TIM_BDTRInitStructure.TIM_OSSRState =TIM_OSSRState_Enable;//����ģʽ�����
	TIM_BDTRInitStructure.TIM_OSSIState =TIM_OSSIState_Enable;//����ģʽ�����
	TIM_BDTRInitStructure.TIM_LOCKLevel =TIM_LOCKLevel_OFF;   //��������,��ֹ���������ṩд����
	TIM_BDTRInitStructure.TIM_DeadTime =0x1f;	              //��������ʱ��0-0xff
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;	  //ɲ��ʹ�ܹر�
    //TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;   //ɲ������ܽż��Ե�(û��ɲ������������䲻��)        //ɲ��������ԣ���ɲ����������PB12��GNDʱ��PWMֹͣ���  
	TIM_BDTRInitStructure.TIM_AutomaticOutput =TIM_AutomaticOutput_Enable; //�Զ��������ʹ��
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);		
		
	//��Ҫ�������������ж����ȼ�����	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;         //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��
	
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //��������ж�����λ
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);    //������ʱ���ж�
	
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1	
}

void TIM1_UP_IRQHandler(void)
{		
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)   //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);      //�ֶ����TIMx���жϴ�����λ:TIM �ж�Դ 	
		
		levelA=(u16)(SPWM[pwmvalA]*M+half_arr);       //������װֵ
		TIM1->CCR1 = levelA;                            //��װTIM1_1��ֵ
		pwmvalA++;
		if(pwmvalA>=400)
			pwmvalA=0;
				
		if(temp_flag==1)//��������λ���־λ��λ
		{
			temp_flag=0;//��λ��־λ
			if((temp>0) && (temp<=10000))//�������ѹ��ǰ������ѹ����������ȥ��ǰ��
			{
				mid=temp/50;
				if(mid>=2)
					mid=2;
				error_PWM_num=pwmvalA-mid;
				if(error_PWM_num>=0)
					pwmvalA-=mid;
				else
					pwmvalA=400+error_PWM_num;	
			}
			else if((temp>=10000) && (temp<=20000))//�������ѹ�ͺ������ѹ�������������ͺ���
			{
				mid=(20000-temp)/50;
				if(mid>=2)
					mid=2;
				error_PWM_num=pwmvalA+mid;
				if(error_PWM_num>=400)
					pwmvalA=error_PWM_num-400;
				else
					pwmvalA+=mid;	
			}
		}		
	}
}	

