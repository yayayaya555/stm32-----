#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"

void Constant_AC_U_PIDControl(u16 rk,u16 yk);//恒逆变输出电压的双向DC/AC的PI调节
void Constant_DC_U_PIDControl(u16 rk,u16 yk);//恒母线电压的双向DC/AC的PI调节

void PV_stable_PIDControl(u16 rk,u16 yk);//固定电压MPPT的PI调节
void Bat_PIDControl(u16 ref,u16 feedback);//rk母线电压给定，yk母线电压反馈
void Bat_Double_PIDControl(u16 ref,u16 feedback1,u16 feedback2);

#endif

