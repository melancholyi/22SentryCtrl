#include "spi.h"
#include "imu.h"



//���ù� ��Ƕ����ֱ�Ӳ���¶ ��ʼ��GPIO��
void SPI_GPIO_Init(){
	GPIO_InitTypeDef  GPIO_InitStruct;
	EXTI_InitTypeDef   EXTI_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
	  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);



  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
	  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStruct);

	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);//PE3 ���ӵ��ж���4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);//PE4 ���ӵ��ж���5
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//�ⲿ�ж�4
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�4
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
}


//���ù� ��Ƕ����ֱ�Ӳ���¶ ��ʼ��TIM10 PWM ����
void SPI_TIM_Init(){
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_TIM10);
    TIM_TimeBaseInitStructure.TIM_Period = 5000-1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);
    TIM_TimeBaseInit(TIM10, &TIM_TimeBaseInitStructure);
    /* TIM10 ��ʼ��PF6 ���ڼ���C�����������¶�Ư������ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_Pulse=0;
    TIM_OC1Init(TIM10, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM10, ENABLE);
    TIM_Cmd(TIM10, ENABLE);
}

//һ����ʼ������  SPI �����������ǵ�
void SPI1_Init(void){    
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef   SPI_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
		//DMA_InitTypeDef dma_spi1_rx;
		//DMA_InitTypeDef dma_spi1_tx;
	
		SPI_GPIO_Init();
		SPI_TIM_Init();
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
		RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE);

			NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
				NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
		
				NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
				NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
				NVIC_Init(&NVIC_InitStructure);	
				
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);

		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 10;
    SPI_Init(SPI1, &SPI_InitStructure);
		SPI_Cmd(SPI1, ENABLE);
				
}
//�жϺ��� ɶҲû��
extern "C" void EXTI4_IRQHandler(void)
{
	
	 if(EXTI_GetITStatus(EXTI_Line4)){
				EXTI_ClearITPendingBit(EXTI_Line4); //���LINE0�ϵ��жϱ�־λ 
				//ACCEL_EXTI_Callback(INT1_ACCEL_Pin);
	 }

}	
//�жϺ��� ɶҲû��
extern "C"  void EXTI9_5_IRQHandler(void)
{
		 if(EXTI_GetITStatus(EXTI_Line5)){
				EXTI_ClearITPendingBit(EXTI_Line5); //���LINE0�ϵ��жϱ�־λ 
			 	//ACCEL_EXTI_Callback(INT1_GRYO_Pin);
  }
}	



