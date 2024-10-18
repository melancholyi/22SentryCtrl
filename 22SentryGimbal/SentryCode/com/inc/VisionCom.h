#ifndef __VISIONCOM_USART_H
#define __VISIONCOM_USART_H

#include "stdio.h" 
#include "stdint.h"    
#include "stm32f4xx_conf.h"
#include "sys.h"

#define Vision_DataLen 32 
//���͵�dma�������
#define VISION_COM_DMA_SEND_STREAM			DMA2_Stream7							//����Usart_TX dma��Ӧ��DMA�����Ĵ��ڵĻ����Լ����������ֲ��һ��
#define VISION_COM_DMA_SEND_CHANNEL			DMA_Channel_4							//����Usart_TX dma��Ӧ��DMAͨ��
#define VISION_COM_DMA_SEND_BUFFER			getSendPackBuff_U1()   						// �����Ҫ�� �����Ƿ��ͻ������������㴴���İ�
#define VISION_COM_DMA_SEND_LENGTH			32										//�����Ƿ��ͳ��ȣ�һ����ǽ��İ���λ�� 32
#define VISION_COM_DMA_SEND_TCFLAG			DMA_FLAG_TCIF7	
		 
#define VISION_COM_RECEIVE_BUFF_LENGTH	40		//���ջ�������С
													//�������Ĵ�С�Լ��޸�	 
#define VISION_COM_DMA_NVICIRQ_CHANNEL		DMA2_Stream2_IRQn  //���Ǵ��ڽ���DMA����  ��dma���ñ����Լ����ڶ�Ӧ����streamX
#define VISION_COM_DMA_REC_STREAM			DMA2_Stream2
#define VISION_COM_DMA_REC_CHANNEL			DMA_Channel_4	  //ͨ��
#define VISION_COM_DMA_FLAG_TCIF			DMA_FLAG_TCIF2	 //���2 ��Ӧstream2 �����Ҫ�޸�Ϊstream7���� DMA_FLAG_TCIF7
#define VISION_COM_DMA_FLAG_TCIF			DMA_FLAG_TCIF2	 //ͬ��	
#define VISION_COM_DMA_IT_TCIF				DMA_IT_TCIF2	//ͬ��
#define VISION_COM_DMA_CLEAR_FLAG			DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2| DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2//ͬ��
#define VISION_COM_USART_PORT				USART1			//����			
#define VISION_COM_USART_IRQ_HANDLER		USART1_IRQHandler //���ڽ����жϺ궨��
#define VISION_COM_USART_NVIC_IRQ_CHANNEL	USART1_IRQn	//�����ж�NVIC�ж�ͨ��
#define VISION_COM_RCC_ENABLE_USART_FUN		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE) //���� RCC��ʼ��
#define VISION_COM_RCC_ENABLE_DMAX_FUN		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)  //���� DMAʱ��ʹ��


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
MSGrecFromUpper_TypeDef* getUpperCtrlInfo(void); //�õ���λ��������Ϣ


//�����ʹ�ú���
#define capacity	32 				//����������
uint8_t* getSendPackBuff_U1(void);
uint8_t* getSendPackBuff_U6(void);
void createPacket(void);
void clearPackData(void);
void loadPackData(MSGsendToUpper_TypeDef* sendPack,bool isColor);
void sendPackData_U1(void);//ʹ�ú���



#endif
