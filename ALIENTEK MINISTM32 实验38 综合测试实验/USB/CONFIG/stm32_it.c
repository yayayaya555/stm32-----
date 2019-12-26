/**
  ******************************************************************************
  * @file    stm32_it.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "stm32_it.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h" 


//删除了其他中断服务函数,只留下USB的两个.
/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN1_TX_IRQHandler(void)
{
	CTR_HP();
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}
/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
	EXTI->PR|=1<<18;//清除USB唤醒中断挂起位 
} 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
