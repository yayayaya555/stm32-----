#include "Display_EPD_W21_Aux.h"

void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;

    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
        if(value & 0x80)
        	EPD_W21_MOSI_1;
        else
        	EPD_W21_MOSI_0;		
        value = (value << 1); 
        EPD_W21_CLK_1;  
    }
}

void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
  // 复位RCC时钟配置(用于调试模式) 
  RCC_DeInit();
 
  // 使能外部晶振 
  RCC_HSEConfig(RCC_HSE_ON);
  
  // 等待外部晶振稳定 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    // 设置锁相环频率PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  }
  else {
    // 使能内部晶振 
    RCC_HSICmd(ENABLE);
    // 等待内置振荡器稳定 
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    // 设置锁相环频率PLLCLK = 8MHz/2 * 16 = 64 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);
  }

    // 使能FLASH的预取缓冲 
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  //设置代码延时值,FLASH_Latency_2为两个延时周期
  FLASH_SetLatency(FLASH_Latency_2);
	
  //设置系统总线时钟
  RCC_HCLKConfig(RCC_SYSCLK_Div1); 

  //设置高速设备总线时钟，RCC_HCLK_Div1为系统时钟除以1
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  //设置低速设备总线时钟，RCC_HCLK_Div2为系统时钟除以2
  RCC_PCLK1Config(RCC_HCLK_Div2);
  
  //使能锁相环倍频
  RCC_PLLCmd(ENABLE);
  
  // 等待锁相环倍频后的频率稳定 
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  
  // 选择锁相环时钟为系统时钟源 
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  // 等待设置完成 
  while(RCC_GetSYSCLKSource() != 0x08);
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
            RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO,
            ENABLE);

}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void NVIC_Configuration(void)
{ 
  //NVIC_InitTypeDef NVIC_InitStructure;
  ;
}

/***********************************************************
						end file
***********************************************************/

