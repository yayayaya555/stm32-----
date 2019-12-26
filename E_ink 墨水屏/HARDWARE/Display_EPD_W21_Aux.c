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
  // ��λRCCʱ������(���ڵ���ģʽ) 
  RCC_DeInit();
 
  // ʹ���ⲿ���� 
  RCC_HSEConfig(RCC_HSE_ON);
  
  // �ȴ��ⲿ�����ȶ� 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    // �������໷Ƶ��PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  }
  else {
    // ʹ���ڲ����� 
    RCC_HSICmd(ENABLE);
    // �ȴ����������ȶ� 
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    // �������໷Ƶ��PLLCLK = 8MHz/2 * 16 = 64 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);
  }

    // ʹ��FLASH��Ԥȡ���� 
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  //���ô�����ʱֵ,FLASH_Latency_2Ϊ������ʱ����
  FLASH_SetLatency(FLASH_Latency_2);
	
  //����ϵͳ����ʱ��
  RCC_HCLKConfig(RCC_SYSCLK_Div1); 

  //���ø����豸����ʱ�ӣ�RCC_HCLK_Div1Ϊϵͳʱ�ӳ���1
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  //���õ����豸����ʱ�ӣ�RCC_HCLK_Div2Ϊϵͳʱ�ӳ���2
  RCC_PCLK1Config(RCC_HCLK_Div2);
  
  //ʹ�����໷��Ƶ
  RCC_PLLCmd(ENABLE);
  
  // �ȴ����໷��Ƶ���Ƶ���ȶ� 
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  
  // ѡ�����໷ʱ��Ϊϵͳʱ��Դ 
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  // �ȴ�������� 
  while(RCC_GetSYSCLKSource() != 0x08);
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
            RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO,
            ENABLE);

}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;    //abcd OE ST LT0���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;    //abcd OE ST LT0���
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

