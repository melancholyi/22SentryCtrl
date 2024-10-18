#ifndef __VISIONCOM_USART_H
#define __VISIONCOM_USART_H

#include "stdio.h" 
#include "stdint.h"    
#include "stm32f4xx_conf.h"
#include "sys.h"

#define Vision_DataLen 32 
//发送的dma配置相关
#define VISION_COM_DMA_SEND_STREAM			DMA2_Stream7							//串口Usart_TX dma对应的DMA流，改串口的话，自己查下数据手册改一下
#define VISION_COM_DMA_SEND_CHANNEL			DMA_Channel_4							//串口Usart_TX dma对应的DMA通道
#define VISION_COM_DMA_SEND_BUFFER			getSendPackBuff_U1()   						// 这个不要改 这里是发送缓冲区，就是你创建的包
#define VISION_COM_DMA_SEND_LENGTH			32										//这里是发送长度，一般就是建的包的位数 32
#define VISION_COM_DMA_SEND_TCFLAG			DMA_FLAG_TCIF7	
		 
#define VISION_COM_RECEIVE_BUFF_LENGTH	40		//接收缓存区大小
													//缓冲区的大小自己修改	 
#define VISION_COM_DMA_NVICIRQ_CHANNEL		DMA2_Stream2_IRQn  //这是串口接收DMA的流  查dma配置表，看自己串口对应的是streamX
#define VISION_COM_DMA_REC_STREAM			DMA2_Stream2
#define VISION_COM_DMA_REC_CHANNEL			DMA_Channel_4	  //通道
#define VISION_COM_DMA_FLAG_TCIF			DMA_FLAG_TCIF2	 //这个2 对应stream2 ，如果要修改为stream7则搞成 DMA_FLAG_TCIF7
#define VISION_COM_DMA_FLAG_TCIF			DMA_FLAG_TCIF2	 //同上	
#define VISION_COM_DMA_IT_TCIF				DMA_IT_TCIF2	//同上
#define VISION_COM_DMA_CLEAR_FLAG			DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2| DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2//同上
#define VISION_COM_USART_PORT				USART1			//串口			
#define VISION_COM_USART_IRQ_HANDLER		USART1_IRQHandler //串口接收中断宏定义
#define VISION_COM_USART_NVIC_IRQ_CHANNEL	USART1_IRQn	//串口中断NVIC中断通道
#define VISION_COM_RCC_ENABLE_USART_FUN		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE) //串口 RCC初始化
#define VISION_COM_RCC_ENABLE_DMAX_FUN		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)  //串口 DMA时钟使能


enum eSentryMode{
	snetry_noCmd_e = 0x00,
	sentry_scout_e   = 	0x3a,
	sentry_autoAim_e = 0x2a,
	sentry_Dbus_e = 0x4a
};

typedef struct 
{
	uint32_t 	packetID;
	eSentryMode sentryMode;
	float 		pitchTarAngel;
	float		yawTarAngel;
	uint8_t 	enemyNum;
	uint8_t 	isShoot;

}MSGrecFromUpper_TypeDef;

typedef struct
{
	uint32_t	packetID;
	uint8_t 	cmd;
	float 		shootSpeed;
	uint8_t 	mode;
	uint8_t  	myColor;
}MSGsendToUpper_TypeDef;

int8_t VISION_COM_SlovedPacket(void);
void VISION_COM_Usart_Init(u32 bound);
MSGrecFromUpper_TypeDef* getUpperCtrlInfo(void); //得到上位机控制信息


//解包的使用函数
#define capacity	32 				//发包的容量
uint8_t* getSendPackBuff_U1(void);
uint8_t* getSendPackBuff_U6(void);
void createPacket(void);
void clearPackData(void);
void loadPackData(MSGsendToUpper_TypeDef* sendPack,bool isColor);
void sendPackData_U1(void);//使用函数



#endif
