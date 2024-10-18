/**MADE BY LIU
  ******************************************************************************
  * @author   LIU_Standard
  * @version  V1.0.0
  * @date     22-Sep-2021
  * @brief    遥控器的接收库，只保留了最最基本的东西，基本只需要配置了中断分组和
  * 		  RC_Init(); 就可以使用遥控，使用只需要rc_ctrl_data能给外部调用即可。
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
RC_CtrlTypeDef rc_ctrl_data; 				//存放遥控器数据作为全局变量调用
/* Private functions --------------------------------------------------------*/

/**
  * @brief  DBUS就是遥控器 串口三的接收端
  * @param  uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num，接收缓冲1 2 和最大空间
  * @retval None
  */
void RC_DeInit(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE); 	//初始化串口3为DBUS

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

	USART_InitStructure.USART_BaudRate = 100000;				//波特率10k
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
  * @brief 处理接收的数据，参考手册代码
  * @param  *pData 数据数组的指针
  * @retval None
*/
void RC_DataProcess(uint8_t *pData)
{
	if(pData == NULL)
	{
		return;
	}
	// ch0~ch3需要都只有11位 0x07FFF = 0111 1111 1111
	// s1~s2  需要2位 C = 1100  3 = 0011
	rc_ctrl_data.rc.ch0 = (((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF)-1024;
	rc_ctrl_data.rc.ch1 = ((((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF)-1024;
	rc_ctrl_data.rc.ch2 = ((((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) | ((int16_t)pData[4] << 10)) & 0x07FF)-1024;
	rc_ctrl_data.rc.ch3 = ((((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF)-1024;
	rc_ctrl_data.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	rc_ctrl_data.rc.s2 = ((pData[5] >> 4) & 0x0003);
	// 键鼠操作未测试2021/9/22
	rc_ctrl_data.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
	rc_ctrl_data.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
	rc_ctrl_data.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);
	rc_ctrl_data.mouse.press_l = pData[12];
	rc_ctrl_data.mouse.press_r = pData[13];
	rc_ctrl_data.key.v = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);
}

/**
  * @brief  返回接收数据包给外部函数使用，当然也可以extern但注意千万不要改
  * @param  None
  * @retval rc_ctrl_data
  */
RC_CtrlTypeDef* getRCInfo(void)
{
	return &rc_ctrl_data;
}

/* Functions -----------------------------------------------------------------*/

/**
  * @brief  初始化，传入接收的数组，大小等等
  * @param  None
  * @retval None
  */
void RC_Dbus_Init(void)
{
	RC_DeInit(rc_rxbuf[0], rc_rxbuf[1], RC_FRAME_LENGTH);
}

/**
  * @brief  串口1中断，处理DMA接收和接收数据的处理，如果在其它地方有使用请注释掉
  * @param  None
  * @retval None
  */
extern "C" void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //串口中断
    {
        USART_ReceiveData(USART3); 
    }
    else if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //IDLE中断
    {
		// 清除IDLE中断（见参考手册USART），实测不清除中断也能使用
		// 9/22能用但是会有数据丢失，有兴趣可自测，对寄存器研究不是很透彻
		(void)USART3->SR;
		(void)USART3->DR;

        if(DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0) // DMA在访问缓冲区0，CPU可访问接收缓冲区1
        {
			//重启DMA接收数据
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
			//重启DMA接收数据
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
