#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

#define KEY0   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) //��ȡM+
#define KEY1   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) //��ȡM-

void KEY_Init(void);	//IO��ʼ��
void KEY_Scan(u8 mode); //����ɨ�躯��	

#endif

