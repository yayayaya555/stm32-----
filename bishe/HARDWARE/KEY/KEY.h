#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

#define KEY0   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) //读取M+
#define KEY1   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) //读取M-

void KEY_Init(void);	//IO初始化
void KEY_Scan(u8 mode); //按键扫描函数	

#endif

