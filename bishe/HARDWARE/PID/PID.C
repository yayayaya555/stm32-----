#include "PID.h"
//a=Kp*(1+Ki+Kd)
//b=Kp*(1+2*Ki)
//c=Kp*Kd

float AC_U_a1=0.5f,AC_U_b1=0.1f,AC_U_duk1,AC_U_duM;
int AC_U_ek,AC_U_ek1;

float Bus_U_a1=0.5f,Bus_U_b1=0.1f,Bus_U_duk1,Bus_U_duM;
int Bus_U_ek,Bus_U_ek1;

float PV_a1=0.15f,PV_b1=0.1f,PV_duk1;
int PV_ek,PV_ek1;

float BAT_a1=0.15f,BAT_b1=0.1f,BAT_duk1;
int BAT_ek,BAT_ek1;

extern float M;
extern u16 PV_Boost_num;
extern u16 BAT_PWM_num;

//�µ����п�������ѹ�ջ�PID����
void Constant_AC_U_PIDControl(u16 rk,u16 yk)//rk��������ѹ������yk��������ѹ����
{
	AC_U_ek=rk-yk;               //���������
	AC_U_duk1=AC_U_a1*AC_U_ek-AC_U_b1*AC_U_ek1;// ����PI�������
	AC_U_ek1=AC_U_ek;            //��ʷֵ��¼
	
	AC_U_duM=AC_U_duk1*0.0003249;   //���õ��ƶȺͷ���������֮��ĺ�����ϵ������Ҫ����ϵ��������PI���ڲ���
	M+=AC_U_duM;					//���㵱ǰ���ƶ�
	if ( M<=0.85 )//PI�������
		M=0.85;
	else if ( M>=0.9999 )	
		M=0.9999;
}
//�������п���ĸ�ߵ�ѹ�ջ�PID����
void Constant_DC_U_PIDControl(u16 rk,u16 yk)//rkֱ��ĸ�ߵ�ѹ������ykֱ��ĸ�ߵ�ѹ����
{
	Bus_U_ek=rk-yk;               //���������
	Bus_U_duk1=Bus_U_a1*Bus_U_ek-Bus_U_b1*Bus_U_ek1;// ����PI�������
	Bus_U_ek1=Bus_U_ek;            //��ʷֵ��¼
	
	Bus_U_duM=Bus_U_duk1*0.0003249;   //���õ��ƶȺͷ���������֮��ĺ�����ϵ������Ҫ����ϵ��������PI���ڲ���
	M-=Bus_U_duM;					//���㵱ǰ���ƶ�
	if ( M<=0.85 )//PI�������
		M=0.85;
	else if ( M>=0.9999 )	
		M=0.9999;
}

//�̶���ѹMPPT��PI����
void PV_stable_PIDControl(u16 rk,u16 yk)//rk������yk����
{
	PV_ek=rk-yk;                        // ���������
	PV_duk1=PV_a1*PV_ek-PV_b1*PV_ek1;	// ����PI�������
	PV_ek1=PV_ek;
	
	PV_Boost_num-=PV_duk1;
	if(PV_Boost_num>1000)//�޷�
		PV_Boost_num=1000;
	if(PV_Boost_num<560) //�޷�
		PV_Boost_num=560;

}

//������ĸ�ߵ�ѹ���ջ�PI����
void Bat_PIDControl(u16 ref,u16 feedback)//rkĸ�ߵ�ѹ������ykĸ�ߵ�ѹ����
{
	BAT_ek=ref-feedback;// ���������error 
	BAT_duk1=BAT_a1*BAT_ek-BAT_b1*BAT_ek1;//����PI�������	
	BAT_ek1=BAT_ek;//Ϊ��һ��������׼��
	
//	BAT_PWM_num+=BAT_duk1;//��������PI���
//	if(BAT_PWM_num>=720)
//		BAT_PWM_num=720;//��������޷�
//	if(BAT_PWM_num<=560)
//		BAT_PWM_num=560;
	
	if((BAT_PWM_num+BAT_duk1)>=720)//���ռ�ձ�50%
		BAT_PWM_num=720;//��������޷�
	else if((BAT_PWM_num+BAT_duk1)>=560)
		BAT_PWM_num+=BAT_duk1;//��������PI���	
	else
		BAT_PWM_num=560;//��������޷�//��Сռ�ձ�33%

}
