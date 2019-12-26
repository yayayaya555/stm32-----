#include "EXT_DAC.h"
#include "delay.h"
#include "myiic.h"

void EXT_Dac_Out(u16 EDval)
{	
	  u8 ED_temp=0;
    IIC_Start();        //Send the Start Bit
    ED_temp=SlaveAddress;//???
    IIC_Send_Byte(ED_temp);
    IIC_Ack();
    ED_temp=(u8)(EDval>>8);//??
    IIC_Send_Byte(ED_temp);
    IIC_Ack();
    ED_temp=(u8)EDval;//??
    IIC_Send_Byte(ED_temp);
    IIC_Ack();
    IIC_Stop();         //Send the Stop condition
}
