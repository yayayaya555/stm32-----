#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"

void Constant_AC_U_PIDControl(u16 rk,u16 yk);//����������ѹ��˫��DC/AC��PI����
void Constant_DC_U_PIDControl(u16 rk,u16 yk);//��ĸ�ߵ�ѹ��˫��DC/AC��PI����

void PV_stable_PIDControl(u16 rk,u16 yk);//�̶���ѹMPPT��PI����
void Bat_PIDControl(u16 ref,u16 feedback);//rkĸ�ߵ�ѹ������ykĸ�ߵ�ѹ����
void Bat_Double_PIDControl(u16 ref,u16 feedback1,u16 feedback2);

#endif
