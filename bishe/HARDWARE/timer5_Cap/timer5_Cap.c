#include "timer5_Cap.h"

u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬
u16	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ

//��ʱ��5ͨ��1���벶������
void TIM5_Cap_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);   //ʹ��TIM5ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ��

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;    //PA0 ���֮ǰ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 ����
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PA0 ����

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;    //PA1 ���֮ǰ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA1 ����
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA1 ����

    //��ʼ����ʱ��5 TIM5
    TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM5���벶�����,ͨ��1
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    //��ʼ��TIM5���벶�����,ͨ��2
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC2ӳ�䵽TI2��
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI2��
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    //�жϷ����ʼ��
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE);//��������ж� ,����CC1IE�����ж�

    TIM_Cmd(TIM5,ENABLE ); 	//ʹ�ܶ�ʱ��5
}

//��ʱ��5�жϷ������
void TIM5_IRQHandler(void)
{
    if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����
    {
        if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
        {
            if(TIM5CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
            {
                if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
                {
                    TIM5CH1_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
                    TIM5CH1_CAPTURE_VAL=0XFFFF;
                } 				
				else 
					TIM5CH1_CAPTURE_STA++;
            }
        }
        if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)//����1���������¼�
        {
			if((TIM5CH1_CAPTURE_STA&0X40)==0)
			{
				TIM5CH1_CAPTURE_STA=0;			//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM_SetCounter(TIM5,0);
				TIM5CH1_CAPTURE_STA|=0X40;		//��ǲ�����������			
			}

        }
        if (TIM_GetITStatus(TIM5, TIM_IT_CC2) != RESET)//����2���������¼�
        {
            if(TIM5CH1_CAPTURE_STA&0X40)
            {
                TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ����������ʱ���
                TIM5CH1_CAPTURE_VAL=TIM_GetCapture2(TIM5);//ͨ��2�ź���ʱ�ļ�ʱ��ֵ
            }
        }			
    }
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update | TIM_IT_CC2); //����жϱ�־λ

}
