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

//孤岛运行控制逆变电压闭环PID调节
void Constant_AC_U_PIDControl(u16 rk,u16 yk)//rk逆变输出电压给定，yk逆变输出电压反馈
{
	AC_U_ek=rk-yk;               //计算误差量
	AC_U_duk1=AC_U_a1*AC_U_ek-AC_U_b1*AC_U_ek1;// 计算PI控制输出
	AC_U_ek1=AC_U_ek;            //历史值记录
	
	AC_U_duM=AC_U_duk1*0.0003249;   //利用调制度和反馈数字量之间的函数关系，仅需要比例系数，修正PI调节参数
	M+=AC_U_duM;					//计算当前调制度
	if ( M<=0.85 )//PI输出饱和
		M=0.85;
	else if ( M>=0.9999 )	
		M=0.9999;
}
//并网运行控制母线电压闭环PID调节
void Constant_DC_U_PIDControl(u16 rk,u16 yk)//rk直流母线电压给定，yk直流母线电压反馈
{
	Bus_U_ek=rk-yk;               //计算误差量
	Bus_U_duk1=Bus_U_a1*Bus_U_ek-Bus_U_b1*Bus_U_ek1;// 计算PI控制输出
	Bus_U_ek1=Bus_U_ek;            //历史值记录
	
	Bus_U_duM=Bus_U_duk1*0.0003249;   //利用调制度和反馈数字量之间的函数关系，仅需要比例系数，修正PI调节参数
	M-=Bus_U_duM;					//计算当前调制度
	if ( M<=0.85 )//PI输出饱和
		M=0.85;
	else if ( M>=0.9999 )	
		M=0.9999;
}

//固定电压MPPT的PI调节
void PV_stable_PIDControl(u16 rk,u16 yk)//rk给定，yk反馈
{
	PV_ek=rk-yk;                        // 计算误差量
	PV_duk1=PV_a1*PV_ek-PV_b1*PV_ek1;	// 计算PI控制输出
	PV_ek1=PV_ek;
	
	PV_Boost_num-=PV_duk1;
	if(PV_Boost_num>1000)//限幅
		PV_Boost_num=1000;
	if(PV_Boost_num<560) //限幅
		PV_Boost_num=560;

}

//蓄电池稳母线电压单闭环PI调节
void Bat_PIDControl(u16 ref,u16 feedback)//rk母线电压给定，yk母线电压反馈
{
	BAT_ek=ref-feedback;// 计算误差量error 
	BAT_duk1=BAT_a1*BAT_ek-BAT_b1*BAT_ek1;//计算PI控制输出	
	BAT_ek1=BAT_ek;//为下一步计算做准备
	
//	BAT_PWM_num+=BAT_duk1;//计算正常PI输出
//	if(BAT_PWM_num>=720)
//		BAT_PWM_num=720;//设置输出限幅
//	if(BAT_PWM_num<=560)
//		BAT_PWM_num=560;
	
	if((BAT_PWM_num+BAT_duk1)>=720)//最大占空比50%
		BAT_PWM_num=720;//设置输出限幅
	else if((BAT_PWM_num+BAT_duk1)>=560)
		BAT_PWM_num+=BAT_duk1;//计算正常PI输出	
	else
		BAT_PWM_num=560;//设置输出限幅//最小占空比33%

}

