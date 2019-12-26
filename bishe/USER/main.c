#include "sys.h"
#include "delay.h"
#include "KEY.h"
#include "adc.h"
#include "pwm.h"
#include "timer2.h"
#include "timer4.h"
#include "timer8.h"
#include "timer5_Cap.h"
#include "usart.h"
#include "PID.h"

void HMI_PV_SEND(void);
void HMI_BAT_BUS_LOAD_SEND(void);
void HMI_AC_SEND(void);
void Mode_Select(void);
void Print_Str(u8 *s);

u8 Voltage_Bus[]="p[2].b[6].txt=\"18.0V\"";
u16 DC_Bus;

u16 PV_Boost_num=720;
u16 PV_Voltage_Average=0, PV_Current_Average=0,PV_Voltage_Ref=2315;//2315->15V
u16 PV_Voltage,PV_Current,PV_Power;
u8 Voltage_PV[]="p[3].b[5].txt=\"18.0V\"";
u8 Current_PV[]="p[3].b[6].txt=\"2.95A\"";
u8 Power_PV[]="p[3].b[7].txt=\"50.0W\"";

u16 Bus_ref=1628;//1632->36V;
u8 Boost_Buck_flag=1;//0->����,1->�ŵ磬2->���
u16 BAT_PWM_num=600;

u16 BAT_Voltage_Average=0, BAT_Current_Average=0,Bus_Voltage_Average=0;
u16 BAT_Voltage,BAT_Current;
u8 Voltage_BAT[]="p[4].b[1].txt=\"18.0V\"";
u8 Current_BAT[]="p[4].b[2].txt=\"2.95A\"";
u8 Discharge1_BAT[]="p[4].b[3].pco=65535";//�ŵ�
u8 Discharge2_BAT[]="p[4].b[8].pco=0";
u8 Discharge3_BAT[]="p[4].b[9].pco=65535";
u8 Charge1_BAT[]="p[4].b[3].pco=0";       //���
u8 Charge2_BAT[]="p[4].b[8].pco=65535";
u8 Charge3_BAT[]="p[4].b[9].pco=65535";
u8 Standby1_BAT[]="p[4].b[3].pco=65535";  //����
u8 Standby2_BAT[]="p[4].b[8].pco=65535";
u8 Standby3_BAT[]="p[4].b[9].pco=0";

u16 Load_Voltage,Load_Current,Load_Power,Load_Current_Average=0;
u8 Voltage_Load[]="p[6].b[5].txt=\"18.0V\"";
u8 Current_Load[]="p[6].b[6].txt=\"2.95A\"";
u8 Power_Load[]="p[6].b[7].txt=\"50.0W\"";

u16 AC_Voltage_Average=0, AC_Current_Average=0, Grid_Voltage_Average=0, AC_Voltage_Ref=2765;//2765->24V
float M=0.9;
u16 Display_M=0;

u16 AC_Voltage,AC_Current;
u8 Voltage_AC[]="p[5].b[1].txt=\"18.0V\"";
u8 Current_AC[]="p[5].b[2].txt=\"2.95A\"";
u8 M_AC[]="p[5].b[9].txt=\"0.999\"";
u8 Grid1_AC[]="p[5].b[3].pco=0";
u8 Grid2_AC[]="p[5].b[8].pco=65535";
u8 Island1_AC[]="p[5].b[3].pco=65535";
u8 Island2_AC[]="p[5].b[8].pco=0";

u8 Phase_flag=0, Voltage_flag=0;//������λ��־λ��������ѹ��־λ
u32 temp=0; 
u8 temp_flag=0, Relay_SwitchOFF_flag=0,flag=0;
extern u8   TIM5CH1_CAPTURE_STA;    //���벶��״̬		    				
extern u16	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ	
u16 Switch_num=0;//��ʱ��բ

u16 Display_flag=0;//Display_flag=1��Ӧ->0.5s,����һ����ʾ
u16 Adjust_flag=0;//Adjust_flag=1��Ӧ->0.1s,����һ��

u8 Island_Grid_flag=1;//1->�µ����У�������ѹ��2->,�������У�������ѹ
u8 Grid_flag=1, Grid_flag_History=1;//Grid_flag=1->����ͣ�磬0->�����е�
int main(void)
 {	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init();	    	//��ʱ������ʼ��
    KEY_Init();				//������ʼ��
	Adc_Init();				//��ʼ��adc1
	TIM1_PWM_Init(10);		//SPWMʵ���ز�Ƶ��10KHz
	TIM2_Int_Init(7199,4);  //ADC�ļ�ʱƵ��=72000/(7199+1)*(4+1)=2KHz
	TIM4_Int_Init(1439,0);  //���BOOST��ʱ��4��Ƶ��=72000/(1439+1)=50KHz
	TIM8_PWM_Init(1439,0);  //����˫��DCDC��ʱ��Ƶ��=72000/(1439+1)=50KHz
	TIM5_Cap_Init(0XFFFF,72-1);	//����Ƚ���������1Mhz��Ƶ�ʼ�����1us 
	uart_init(115200);
   	
	while(1)
	{
		//KEY_Scan(0);//�ֶ�ת�������µ���key1�µ���key2����
	
		if(Adjust_flag==1)//ÿ��0.05s����һ��
		{				
			Adjust_flag=0;
			
			Mode_Select();//�Զ��л������µ��������е粢��������ͣ��µ�
			
			PV_stable_PIDControl(PV_Voltage_Ref,PV_Voltage_Average);//�����ѹ����ʸ���

			if(Island_Grid_flag==1)
				Bat_PIDControl(Bus_ref,Bus_Voltage_Average);//�µ����У�������ĸ�ߵ�ѹ				

			if(Relay_SwitchOFF_flag==1)//��ʱ�������
			{
				//������ѹ��������
				if(((AC_Voltage_Average-Grid_Voltage_Average)<50) && ((AC_Voltage_Average-Grid_Voltage_Average)>-50))
					Voltage_flag=1;
				else
					Voltage_flag=0;
				//������λ��������
				if((temp<100) || ((20000-temp)<100))
					Phase_flag=1;		
				else
					Phase_flag=0;
				
				//������־λͬʱ��������
				if(Voltage_flag && Phase_flag)
					Switch_num++;		
				else
					Switch_num=0;
				
				if(Switch_num==20)
				{	
					Relay_SwitchOFF_flag=0;
					Island_Grid_flag=0;//�������б�־λ
					TIM8_CC0_Init();//����ǰ�����ش���
					
					delay_ms(300);//��������ʱ��
					
					Switch_num=0;//Ϊ��һ�κ�բ��׼��
					flag=1;//�´��բָ��
					GPIO_SetBits(GPIOA,GPIO_Pin_2);//��������
				}
			}
			if(flag==1)//������ĸ�ߵ�ѹ����36V
				Constant_DC_U_PIDControl(Bus_ref,Bus_Voltage_Average);
			else//�µ�����������ѹ����24V
				Constant_AC_U_PIDControl(AC_Voltage_Ref,AC_Voltage_Average);
				
		}
		
		if(Display_flag==1)//��ʾÿ��0.5s����һ�Σ�����ȱ���ж�ÿ��0.5s�ж�һ��
		{
			HMI_PV_SEND();
			HMI_BAT_BUS_LOAD_SEND();
			HMI_AC_SEND();
			Display_flag=0;
		}
		
		if(TIM5CH1_CAPTURE_STA&0X80)//�ɹ�������һ��������
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F;
			temp*=65536;//���ʱ���ܺ�
			temp+=TIM5CH1_CAPTURE_VAL;//�õ�������ʱ���

			TIM5CH1_CAPTURE_STA=0;//������һ�β���
			temp_flag=1;
		}
		
	} 
}
 
void HMI_PV_SEND(void)
{
	PV_Voltage=(u16)((0.006495*PV_Voltage_Average-0.08167)*10);
	PV_Current=(u16)((0.003820*PV_Current_Average-11.86)*100);
	
	PV_Power=PV_Voltage*PV_Current;
	
	Voltage_PV[15]=PV_Voltage/100+'0';
	Voltage_PV[16]=PV_Voltage%100/10+'0';
	Voltage_PV[18]=PV_Voltage%10+'0';

	Current_PV[15]=PV_Current/100+'0';
	Current_PV[17]=PV_Current%100/10+'0';
	Current_PV[18]=PV_Current%10+'0';

	Power_PV[15]=PV_Power/10000+'0';
	Power_PV[16]=PV_Power%10000/1000+'0';
	Power_PV[18]=PV_Power%1000/100+'0';

	Print_Str(Voltage_PV);
	Print_Str(Current_PV);
	Print_Str(Power_PV);
}

void HMI_BAT_BUS_LOAD_SEND(void)
{
	BAT_Voltage=(u16)((0.01508*BAT_Voltage_Average+0.1018)*10);
	DC_Bus=(u16)((0.02182*Bus_Voltage_Average+0.3695)*10);
	
	if(Island_Grid_flag==1)
	{
		if((BAT_Current_Average-3061)>=0) //�ŵ�
			Boost_Buck_flag=1;
		else
			Boost_Buck_flag=2;	
	}
	else
		Boost_Buck_flag=0;

	if(Boost_Buck_flag==1) //BOOST�ŵ�
	{
		BAT_Current=(u16)((0.00793*BAT_Current_Average-24.28)*100);
		Print_Str(Discharge1_BAT);
		Print_Str(Discharge2_BAT);
		Print_Str(Discharge3_BAT);
	}
	else if(Boost_Buck_flag==2)//BUCK���
	{
		BAT_Current=(u16)((24.28-0.00793*BAT_Current_Average)*100);
		Print_Str(Charge1_BAT);
		Print_Str(Charge2_BAT);
		Print_Str(Charge3_BAT);
	}
	else//����
	{
		BAT_Current=0;
		Print_Str(Standby1_BAT);
		Print_Str(Standby2_BAT);
		Print_Str(Standby3_BAT);
	}
	
	Voltage_BAT[15]=BAT_Voltage/100+'0';
	Voltage_BAT[16]=BAT_Voltage%100/10+'0';
	Voltage_BAT[18]=BAT_Voltage%10+'0';
	
	Current_BAT[15]=BAT_Current/100+'0';
	Current_BAT[17]=BAT_Current%100/10+'0';
	Current_BAT[18]=BAT_Current%10+'0';
	
	Voltage_Bus[15]=DC_Bus/100+'0';
	Voltage_Bus[16]=DC_Bus%100/10+'0';
	Voltage_Bus[18]=DC_Bus%10+'0';
	
	Load_Current=(u16)((0.004118*Load_Current_Average-12.69)*100);
	Load_Power=DC_Bus*Load_Current;
	
	Voltage_Load[15]=DC_Bus/100+'0';
	Voltage_Load[16]=DC_Bus%100/10+'0';
	Voltage_Load[18]=DC_Bus%10+'0';
	
	Current_Load[15]=Load_Current/100+'0';
	Current_Load[17]=Load_Current%100/10+'0';
	Current_Load[18]=Load_Current%10+'0';
	
	Power_Load[15]=Load_Power/10000+'0';
	Power_Load[16]=Load_Power%10000/1000+'0';
	Power_Load[18]=Load_Power%1000/100+'0';

	Print_Str(Voltage_BAT);
	Print_Str(Current_BAT);
	Print_Str(Voltage_Bus);
	Print_Str(Voltage_Load);
	Print_Str(Current_Load);
	Print_Str(Power_Load);
}

void HMI_AC_SEND(void)
{
	AC_Voltage=(u16)((0.008415*AC_Voltage_Average+0.8135)*10);
	AC_Current=(u16)((0.0006894*AC_Current_Average+0.01708)*100);
	if(flag==1)//flag==1��������flag==0���µ�
	{
		Print_Str(Grid1_AC);
		Print_Str(Grid2_AC);
	}
	else
	{
		Print_Str(Island1_AC);
		Print_Str(Island2_AC);
	}
	Voltage_AC[15]=AC_Voltage/100+'0';
	Voltage_AC[16]=AC_Voltage%100/10+'0';
	Voltage_AC[18]=AC_Voltage%10+'0';

	Current_AC[15]=AC_Current/100+'0';
	Current_AC[17]=AC_Current%100/10+'0';
	Current_AC[18]=AC_Current%10+'0';
	
	Display_M=(u16)(M*1000);
	M_AC[17]=Display_M/100+'0';
	M_AC[18]=Display_M%100/10+'0';
	M_AC[19]=Display_M%10+'0';

	Print_Str(Voltage_AC);
	Print_Str(Current_AC);
	Print_Str(M_AC);
}

void Mode_Select(void)
{
	if(Grid_Voltage_Average>2000)
		Grid_flag=0;//����
	else
		Grid_flag=1;//�µ�
	if(Grid_flag != Grid_flag_History)//����Ƶ���ж�
	{
		Grid_flag_History = Grid_flag;//������ʷֵ
		if(Grid_flag==1)//�µ�ǰ��׼��
		{
			Island_Grid_flag=1;//�µ�����
			flag=0;
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);//������բ
			TIM8_CC1_Init();//��ع���
		}
		else//����ǰ��׼��
		{
			Relay_SwitchOFF_flag=1;//������բָ��
		}
		
	}
	
}
void Print_Str(u8 *s)
{
    u8 i;
	while(*s != '\0')
    {
        USART1->DR=(*s++);
		while((USART1->SR&0X40)==0);//�ȴ����ͽ���
    }
	for(i=0;i<3;i++)
	{
		USART1->DR=0xff;
		while((USART1->SR&0X40)==0);//�ȴ����ͽ���
	}

}
