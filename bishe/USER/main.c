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
u8 Boost_Buck_flag=1;//0->待机,1->放电，2->充电
u16 BAT_PWM_num=600;

u16 BAT_Voltage_Average=0, BAT_Current_Average=0,Bus_Voltage_Average=0;
u16 BAT_Voltage,BAT_Current;
u8 Voltage_BAT[]="p[4].b[1].txt=\"18.0V\"";
u8 Current_BAT[]="p[4].b[2].txt=\"2.95A\"";
u8 Discharge1_BAT[]="p[4].b[3].pco=65535";//放电
u8 Discharge2_BAT[]="p[4].b[8].pco=0";
u8 Discharge3_BAT[]="p[4].b[9].pco=65535";
u8 Charge1_BAT[]="p[4].b[3].pco=0";       //充电
u8 Charge2_BAT[]="p[4].b[8].pco=65535";
u8 Charge3_BAT[]="p[4].b[9].pco=65535";
u8 Standby1_BAT[]="p[4].b[3].pco=65535";  //待机
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

u8 Phase_flag=0, Voltage_flag=0;//并网相位标志位，并网电压标志位
u32 temp=0; 
u8 temp_flag=0, Relay_SwitchOFF_flag=0,flag=0;
extern u8   TIM5CH1_CAPTURE_STA;    //输入捕获状态		    				
extern u16	TIM5CH1_CAPTURE_VAL;	//输入捕获值	
u16 Switch_num=0;//延时合闸

u16 Display_flag=0;//Display_flag=1对应->0.5s,更新一次显示
u16 Adjust_flag=0;//Adjust_flag=1对应->0.1s,调整一次

u8 Island_Grid_flag=1;//1->孤岛运行，蓄电池稳压；2->,并网运行，电网稳压
u8 Grid_flag=1, Grid_flag_History=1;//Grid_flag=1->电网停电，0->电网有电
int main(void)
 {	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init();	    	//延时函数初始化
    KEY_Init();				//按键初始化
	Adc_Init();				//初始化adc1
	TIM1_PWM_Init(10);		//SPWM实际载波频率10KHz
	TIM2_Int_Init(7199,4);  //ADC的计时频率=72000/(7199+1)*(4+1)=2KHz
	TIM4_Int_Init(1439,0);  //光伏BOOST定时器4的频率=72000/(1439+1)=50KHz
	TIM8_PWM_Init(1439,0);  //蓄电池双向DCDC定时器频率=72000/(1439+1)=50KHz
	TIM5_Cap_Init(0XFFFF,72-1);	//过零比较器捕获以1Mhz的频率计数，1us 
	uart_init(115200);
   	
	while(1)
	{
		//KEY_Scan(0);//手动转化并网孤岛，key1孤岛，key2并网
	
		if(Adjust_flag==1)//每隔0.05s调节一次
		{				
			Adjust_flag=0;
			
			Mode_Select();//自动切换并网孤岛，电网有电并网，电网停电孤岛
			
			PV_stable_PIDControl(PV_Voltage_Ref,PV_Voltage_Average);//光伏恒压最大功率跟踪

			if(Island_Grid_flag==1)
				Bat_PIDControl(Bus_ref,Bus_Voltage_Average);//孤岛运行，蓄电池稳母线电压				

			if(Relay_SwitchOFF_flag==1)//延时并网检测
			{
				//并网电压满足条件
				if(((AC_Voltage_Average-Grid_Voltage_Average)<50) && ((AC_Voltage_Average-Grid_Voltage_Average)>-50))
					Voltage_flag=1;
				else
					Voltage_flag=0;
				//并网相位满足条件
				if((temp<100) || ((20000-temp)<100))
					Phase_flag=1;		
				else
					Phase_flag=0;
				
				//两个标志位同时满足条件
				if(Voltage_flag && Phase_flag)
					Switch_num++;		
				else
					Switch_num=0;
				
				if(Switch_num==20)
				{	
					Relay_SwitchOFF_flag=0;
					Island_Grid_flag=0;//并网运行标志位
					TIM8_CC0_Init();//并网前将蓄电池待机
					
					delay_ms(300);//并网缓冲时间
					
					Switch_num=0;//为下一次合闸作准备
					flag=1;//下达合闸指令
					GPIO_SetBits(GPIOA,GPIO_Pin_2);//立即并网
				}
			}
			if(flag==1)//并网稳母线电压控制36V
				Constant_DC_U_PIDControl(Bus_ref,Bus_Voltage_Average);
			else//孤岛稳逆变输出电压控制24V
				Constant_AC_U_PIDControl(AC_Voltage_Ref,AC_Voltage_Average);
				
		}
		
		if(Display_flag==1)//显示每隔0.5s更新一次，功率缺额判断每隔0.5s判断一次
		{
			HMI_PV_SEND();
			HMI_BAT_BUS_LOAD_SEND();
			HMI_AC_SEND();
			Display_flag=0;
		}
		
		if(TIM5CH1_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F;
			temp*=65536;//溢出时间总和
			temp+=TIM5CH1_CAPTURE_VAL;//得到两脉冲时间差

			TIM5CH1_CAPTURE_STA=0;//开启下一次捕获
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
		if((BAT_Current_Average-3061)>=0) //放电
			Boost_Buck_flag=1;
		else
			Boost_Buck_flag=2;	
	}
	else
		Boost_Buck_flag=0;

	if(Boost_Buck_flag==1) //BOOST放电
	{
		BAT_Current=(u16)((0.00793*BAT_Current_Average-24.28)*100);
		Print_Str(Discharge1_BAT);
		Print_Str(Discharge2_BAT);
		Print_Str(Discharge3_BAT);
	}
	else if(Boost_Buck_flag==2)//BUCK充电
	{
		BAT_Current=(u16)((24.28-0.00793*BAT_Current_Average)*100);
		Print_Str(Charge1_BAT);
		Print_Str(Charge2_BAT);
		Print_Str(Charge3_BAT);
	}
	else//待机
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
	if(flag==1)//flag==1，并网；flag==0，孤岛
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
		Grid_flag=0;//并网
	else
		Grid_flag=1;//孤岛
	if(Grid_flag != Grid_flag_History)//避免频繁判断
	{
		Grid_flag_History = Grid_flag;//保存历史值
		if(Grid_flag==1)//孤岛前期准备
		{
			Island_Grid_flag=1;//孤岛运行
			flag=0;
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);//立即分闸
			TIM8_CC1_Init();//电池工作
		}
		else//并网前期准备
		{
			Relay_SwitchOFF_flag=1;//发出合闸指令
		}
		
	}
	
}
void Print_Str(u8 *s)
{
    u8 i;
	while(*s != '\0')
    {
        USART1->DR=(*s++);
		while((USART1->SR&0X40)==0);//等待发送结束
    }
	for(i=0;i<3;i++)
	{
		USART1->DR=0xff;
		while((USART1->SR&0X40)==0);//等待发送结束
	}

}
