#include "pid.h"
//a=Kp*(1+Ki+Kd)
//b=Kp*(1+2*Ki)
//c=Kp*Kd

float a1=0.128f,b1=0.1f,c1=0,duk1,duM;
int ek,ek1,ek2;

extern float M;

//电压闭环PID调节
void u_PIDControl(int rk,int yk)//rk给定，yk反馈
{
	ek=rk-yk;
	duk1=a1*ek+b1*ek1+c1*ek2;		// 计算控制输出
	ek2=ek1; ek1=ek;
	//if ( duk1>80 )	duk1=80;		// 幅度限制
	duM=duk1*0.0003829; //利用调制度和反馈数字量之间的函数关系，仅需要比例系数，修正PI调节参数
	M+=duM;					// 计算当前电流环给定值
	if ( M<=0.8 )	M=0.8;
	else if ( M>=0.98 )	M=0.98;//ACR饱和
}



