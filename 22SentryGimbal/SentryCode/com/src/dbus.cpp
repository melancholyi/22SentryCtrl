/**MADE BY LIU
  ******************************************************************************
  * @author   LIU_Standard
  * @version  V1.0.0
  * @date     22-Sep-2021
  * @brief    ң�����Ľ��տ⣬ֻ��������������Ķ���������ֻ��Ҫ�������жϷ����
  * 		  RC_Init(); �Ϳ���ʹ��ң�أ�ʹ��ֻ��Ҫrc_ctrl_data�ܸ��ⲿ���ü��ɡ�
  * 
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Private Includes ----------------------------------------------------------*/
#include "dbus.h" 
#include "gm2ch.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/ 
#define RC_FRAME_LENGTH (18u)

/* Global param ------------------------------------------------------------*/
static uint8_t rc_rxbuf[2][RC_FRAME_LENGTH];
RC_CtrlTypeDef rc_ctrl_data; 				//���ң����������Ϊȫ�ֱ�������
/* Private functions --------------------------------------------------------*/

/**
  * @brief  DBUS����ң���� �������Ľ��ն�
  * @param  uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num�����ջ���1 2 �����ռ�
  * @retval None
  */
void RC_DeInit(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE); 	//��ʼ������3ΪDBUS

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3); 	//PC11  usart3 rx
	/* -------------- Configure GPIO ---------------------------------------*/
	{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_DeInit(USART3);

	USART_InitStructure.USART_BaudRate = 100000;				//������10k
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3, &USART_InitStructure);

	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

	USART_ClearFlag(USART3, USART_FLAG_IDLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

	USART_Cmd(USART3, ENABLE);
	}
	
	/* -------------- Configure NVIC ---------------------------------------*/
	{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	}
	
	/* -------------- Configure DMA -----------------------------------------*/
	{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Stream1);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART3->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rx1_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = dma_buf_num;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream1, &DMA_InitStructure);
	DMA_DoubleBufferModeConfig(DMA1_Stream1, (uint32_t)rx2_buf, DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA1_Stream1, ENABLE);
	DMA_Cmd(DMA1_Stream1, DISABLE);
	DMA_Cmd(DMA1_Stream1, ENABLE);
	}    
	
	while( 1 ){
		if( (rc_ctrl_data.rc.ch0 == 660) || (rc_ctrl_data.rc.ch1 == 660) || 
			(rc_ctrl_data.rc.ch2 == 660) || (rc_ctrl_data.rc.ch3 == 660) ||
			(rc_ctrl_data.rc.ch0 == -660) || (rc_ctrl_data.rc.ch1 == -660) || 
			(rc_ctrl_data.rc.ch2 == -660) || (rc_ctrl_data.rc.ch3 == -660))
		{
			break;
		}
	}
}

/**
  * @brief ������յ����ݣ��ο��ֲ����
  * @param  *pData ���������ָ��
  * @retval None
*/
void RC_DataProcess(uint8_t *pData)
{
	if(pData == NULL)
	{
		return;
	}
	// ch0~ch3��Ҫ��ֻ��11λ 0x07FFF = 0111 1111 1111
	// s1~s2  ��Ҫ2λ C = 1100  3 = 0011
	rc_ctrl_data.rc.ch0 = (((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF)-1024;
	rc_ctrl_data.rc.ch1 = ((((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF)-1024;
	rc_ctrl_data.rc.ch2 = ((((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) | ((int16_t)pData[4] << 10)) & 0x07FF)-1024;
	rc_ctrl_data.rc.ch3 = ((((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF)-1024;
	rc_ctrl_data.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	rc_ctrl_data.rc.s2 = ((pData[5] >> 4) & 0x0003);
	// �������δ����2021/9/22
	rc_ctrl_data.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
	rc_ctrl_data.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
	rc_ctrl_data.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);
	rc_ctrl_data.mouse.press_l = pData[12];
	rc_ctrl_data.mouse.press_r = pData[13];
	rc_ctrl_data.key.v = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);
}

/**
  * @brief  ���ؽ������ݰ����ⲿ����ʹ�ã���ȻҲ����extern��ע��ǧ��Ҫ��
  * @param  None
  * @retval rc_ctrl_data
  */
RC_CtrlTypeDef* getRCInfo(void)
{
	return &rc_ctrl_data;
}

/* Functions -----------------------------------------------------------------*/

/**
  * @brief  ��ʼ����������յ����飬��С�ȵ�
  * @param  None
  * @retval None
  */
void RC_Dbus_Init(void)
{
	RC_DeInit(rc_rxbuf[0], rc_rxbuf[1], RC_FRAME_LENGTH);
}

/**
  * @brief  ����1�жϣ�����DMA���պͽ������ݵĴ�������������ط���ʹ����ע�͵�
  * @param  None
  * @retval None
  */
extern "C" void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�����ж�
    {
        USART_ReceiveData(USART3); 
    }
    else if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //IDLE�ж�
    {
		// ���IDLE�жϣ����ο��ֲ�USART����ʵ�ⲻ����ж�Ҳ��ʹ��
		// 9/22���õ��ǻ������ݶ�ʧ������Ȥ���Բ⣬�ԼĴ����о����Ǻ�͸��
		(void)USART3->SR;
		(void)USART3->DR;

        if(DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0) // DMA�ڷ��ʻ�����0��CPU�ɷ��ʽ��ջ�����1
        {
			//����DMA��������
            DMA_Cmd(DMA1_Stream1, DISABLE);
			DMA_SetCurrDataCounter(DMA1_Stream1, RC_FRAME_LENGTH);
			DMA_DoubleBufferModeConfig(DMA1_Stream1, (uint32_t)rc_rxbuf[1], DMA_Memory_1);
            DMA_Cmd(DMA1_Stream1, ENABLE);
			
			if(DMA_GetCurrDataCounter(DMA1_Stream1) == RC_FRAME_LENGTH)
			{
				RC_DataProcess(rc_rxbuf[1]);
			}
        }
        else
        {
			//����DMA��������
            DMA_Cmd(DMA1_Stream1, DISABLE);
			DMA_SetCurrDataCounter(DMA1_Stream1, RC_FRAME_LENGTH);
			DMA_DoubleBufferModeConfig(DMA1_Stream1, (uint32_t)rc_rxbuf[1], DMA_Memory_0);
            DMA_Cmd(DMA1_Stream1, ENABLE);
			
			if(DMA_GetCurrDataCounter(DMA1_Stream1) == RC_FRAME_LENGTH)
			{
				RC_DataProcess(rc_rxbuf[0]);
			}
        }
    }
}

/*****************************END OF FILE*********************************/
