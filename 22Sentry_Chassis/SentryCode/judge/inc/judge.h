/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :�����ñ�
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  *
  * ������־��
  * 2021��1��10�գ����ҵ�judge.c�ĵ�512�У�USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);//��������DMA
  * Ӧ��ΪUSART_DMACmd(Judge_UseUART_Typedef_Name,USART_DMAReq_Tx,ENABLE);
  *
  *	2021��4��3�գ��յ����°�Ĳ���ϵͳ����ʼ��ֲ��
  *2021.12.05
  */

#ifndef __JUDGE_H__
#define __JUDGE_H__

#include "judgeStruct.h"
#include "CRCCheck.h"

#ifdef __cplusplus
extern "C"{
#endif	/*__cplusplus*/
/********************���ں�DMA�������ñ�***********************/

//������أ�����RX,TX����Ϊ����ϵͳ����IO�ڶ˿ں�����
#define Judge_UseGPIO_GPIO_PeriphClock RCC_AHB1Periph_GPIOB		
#define Judge_UseGPIO_GPIO_Typedef_Name	GPIOB
#define Judge_UseGPIO_GPIO_PINSource_TX	GPIO_PinSource10		
#define Judge_UseGPIO_GPIO_GPIO_Pin_TX	GPIO_Pin_10
#define Judge_UseGPIO_GPIO_PINSource_RX	GPIO_PinSource11
#define Judge_UseGPIO_GPIO_GPIO_Pin_RX	GPIO_Pin_11

//������أ����Ĵ���Ϊ��ʹ�ô�������
#define Judge_UseUART_PeriphClock	RCC_APB1Periph_USART3
#define Judge_UseUART_GPIOAF_Name GPIO_AF_USART3
#define Judge_UseUART_Typedef_Name	USART3
#define Judge_UseUART_IRQn	USART3_IRQn
#define Judge_UseUART_IRQHandler	USART3_IRQHandler

//��������أ���������Ϊ��ǰ����
#define Judge_UseUART_Function_Name	USART3_Init
#define Judge_UseUART_SendChar	USART3_SendChar


//DMA�������
//RX��DMA
#define Judge_UseDMA_PeriphClock_RX	RCC_AHB1Periph_DMA1				//���ָ�Ϊ��ǰ����RX��DMA����
#define Judge_UseDMA_IRQChannel_RX	DMA1_Stream1_IRQn				//���ָ�Ϊ��ӦDMA������������
#define Judge_UseDMA_Stream_Typedef_Name_RX	DMA1_Stream1			//���ָ�Ϊ����������
#define Judge_UseDMA_ClearIT_RX	DMA_IT_TCIF1						//���ָ�Ϊ����������
#define Judge_UseDMA_Channel_RX	DMA_Channel_4						//���ָ�Ϊͨ������
#define Judge_UseDMA_ClearFlag	DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1		//���ָ�Ϊͨ������
//����
#define Judge_UseDMA_IRQHandler_RX			DMA1_Stream1_IRQHandler	//���ָ�Ϊ��ӦDMA������������
//TX��DMA
#define Judge_UseDMA_PeriphClock_TX			RCC_AHB1Periph_DMA1		//���ָ�Ϊ��ǰ����TX��DMA����
#define Judge_UseDMA_IRQChannel_TX			DMA1_Stream3_IRQn		//���ָ�Ϊ��ӦDMA������������
#define Judge_UseDMA_Stream_Typedef_Name_TX	DMA1_Stream3			//���ָ�Ϊ��ӦDMA������������
#define Judge_UseDMA_ClearIT_TX				DMA_IT_TCIF3			//���ָ�Ϊ����������
#define Judge_UseDMA_Channel_TX				DMA_Channel_4  			//���ָ�Ϊͨ������
//����
#define Judge_UseDMA_IRQHandler_TX	DMA1_Stream3_IRQHandler			//���ָ�Ϊ��ӦDMA������������




///////////////////////////////////////////////////////////////���ñ�
/********************4·װ�װ����ñ����ʹ�ã�һ��Ҫ��GRAPH_USE_YAW_OR_ARMOR����Ϊ1***********************/
#define JUDGE_GRAPH_DISTANCE	200.0f		//װ�װ嵽��ת���ľ���
#define JUDGE_GRAPH_ROTATE_X	960.0f		//��ת����X����
#define JUDGE_GRAPH_ROTATE_Y	540.0f		//��ת����Y����
#define JUDGE_GRAPH_BOARD_LENGTH	50.0f	//װ�װ峤��
#define JUDGE_GRAPH_BOARD_WIDTH	20.0f		//װ�װ���


/********************7·�ַ���ͨ�����ñ�***********************/
#define CHAR_SIZE_CH1	15
#define WIDTH_CH1      2
#define COLOR_CH1      Yellow
#define POSITION_X_CH1  50  
#define POSITION_Y_CH1  880
#define SPACE_Y_CHANNEL	30

/********************7·ͼ��ͨ�����ñ�***********************/
//��������
#define GRAPH_CHANNEL_ALL_RADIUS	8	//Բ�뾶
#define GRAPH_CHANNEL_ALL_LENGTH	20	//���γ���
#define GRAPH_CHANNEL_STR_COLOR_ALL	Green
#define GRAPH_CHANNEL_STR_LARGE_ALL	20		//�ַ��������С
#define GRAPH_CHANNEL_STR_WIDTH_ALL	2		//�ַ����߿�

//��������
#define GRAPH_CHANNEL_DELT_POSITION  70 //ͼ�μ��

#define GRAPH_USE_THIS_GRAPH_8_1		1		//���������Ϊ1��ʾʹ�����ͼ�Σ�Ϊ0��ʹ�����ͼ��
#define GRAPH_CHANNEL_8_1_POSITION_X	750
#define GRAPH_CHANNEL_8_1_POSITION_Y	1080-300
#define GRAPH_CHANNEL_8_1_WIDTH			20	//�������
#define GRAPH_CHANNEL_STR_8_1_POSITION_X	GRAPH_CHANNEL_8_1_POSITION_X		//ɫ��1˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_1_POSITION_Y	GRAPH_CHANNEL_8_1_POSITION_Y - 20	//ɫ��1˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_1	"F"	//ɫ��1����ʾ˵���ַ���

#define GRAPH_USE_THIS_GRAPH_8_2		1		//���������Ϊ1��ʾʹ�����ͼ�Σ�Ϊ0��ʹ�����ͼ��
#define GRAPH_CHANNEL_8_2_POSITION_X	GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION
#define GRAPH_CHANNEL_8_2_POSITION_Y	1080-300
#define GRAPH_CHANNEL_8_2_WIDTH			20	//�������
#define GRAPH_CHANNEL_STR_8_2_POSITION_X	GRAPH_CHANNEL_8_2_POSITION_X		//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_2_POSITION_Y	GRAPH_CHANNEL_8_2_POSITION_Y - 20 //ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_2	"D"	//ɫ�����ʾ˵���ַ���

#define GRAPH_USE_THIS_GRAPH_8_3		1		//���������Ϊ1��ʾʹ�����ͼ�Σ�Ϊ0��ʹ�����ͼ��
#define GRAPH_CHANNEL_8_3_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*2)
#define GRAPH_CHANNEL_8_3_POSITION_Y	1080-300
#define GRAPH_CHANNEL_8_3_WIDTH			20	//������ȡ�
#define GRAPH_CHANNEL_STR_8_3_POSITION_X	GRAPH_CHANNEL_8_3_POSITION_X		//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_3_POSITION_Y	GRAPH_CHANNEL_8_3_POSITION_Y - 20	//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_3	"C"	//ɫ�����ʾ˵���ַ���

#define GRAPH_USE_THIS_GRAPH_8_4		0		//���������Ϊ1��ʾʹ�����ͼ�Σ�Ϊ0��ʹ�����ͼ��
#define GRAPH_CHANNEL_8_4_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*3)
#define GRAPH_CHANNEL_8_4_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_4_WIDTH			20	//������ȡ�
#define GRAPH_CHANNEL_STR_8_4_POSITION_X	GRAPH_CHANNEL_8_4_POSITION_X		//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_4_POSITION_Y	GRAPH_CHANNEL_8_4_POSITION_Y	//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_4	"GRAPH_1"	//ɫ�����ʾ˵���ַ���

#define GRAPH_USE_THIS_GRAPH_8_5		0		//���������Ϊ1��ʾʹ�����ͼ�Σ�Ϊ0��ʹ�����ͼ��
#define GRAPH_CHANNEL_8_5_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*4)
#define GRAPH_CHANNEL_8_5_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_5_WIDTH			20	//������ȡ�
#define GRAPH_CHANNEL_STR_8_5_POSITION_X	GRAPH_CHANNEL_8_5_POSITION_X	//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_5_POSITION_Y	GRAPH_CHANNEL_8_5_POSITION_Y	//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_5	"GRAPH_1"	//ɫ�����ʾ˵���ַ���

#define GRAPH_USE_THIS_GRAPH_8_6		0		//���������Ϊ1��ʾʹ�����ͼ�Σ�Ϊ0��ʹ�����ͼ��
#define GRAPH_CHANNEL_8_6_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*5)
#define GRAPH_CHANNEL_8_6_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_6_WIDTH			20	//������ȡ�
#define GRAPH_CHANNEL_STR_8_6_POSITION_X	GRAPH_CHANNEL_8_6_POSITION_X		//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_6_POSITION_Y	GRAPH_CHANNEL_8_6_POSITION_Y	//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_6	"GRAPH_1"	//ɫ�����ʾ˵���ַ���

#define GRAPH_USE_THIS_GRAPH_8_7		0		//���������Ϊ1��ʾʹ�����ͼ�Σ�Ϊ0��ʹ�����ͼ��
#define GRAPH_CHANNEL_8_7_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*6)
#define GRAPH_CHANNEL_8_7_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_7_WIDTH			20	//������ȡ�
#define GRAPH_CHANNEL_STR_8_7_POSITION_X	GRAPH_CHANNEL_8_7_POSITION_X		//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_7_POSITION_Y	GRAPH_CHANNEL_8_7_POSITION_Y	//ɫ��˵���ַ���X����
#define GRAPH_CHANNEL_STR_8_7	"GRAPH_1"	//ɫ�����ʾ˵���ַ���


/********************YAW���PITCH�����ñ�***********************/
#define GRAPH_USE_YAW_OR_ARMOR	0	//0��ʱ��ʹ��YAW�ᣬ1ʱ��ʹ��YAW�ᣬʹ��װ�װ���ת

#define CHAR_SIZE_YAW_PIT	16
#define WIDTH_YAW_PIT      2
#define COLOR_YAW_PIT      Green
#define POSITION_X__YAW_PIT  580  
#define POSITION_Y__YAW_PIT  540



/********************��ӡ�����������ñ�***********************/
#define CHAR_SIZE_VOLTAGE_2	14	//��ѹ�ַ���С
#define WIDTH_VOLTAGE_2      2	//��ѹ�ַ��߿�
#define COLOR_VOLTAGE_2      Green	//��ѹ�ַ���ɫ
#define VOLTAGE_DONT_USE	0		//������������Ϊ1�����ǲ�Ҫ����������ʾ�ĵ�ѹ
#define POSITION_X_CAP  505
#define POSITION_Y_CAP  180
#define RECTANGLE_WIDTH	2	//�����߿��ȣ�����ҪΪż������Ȼ����治�ÿ�������Ҫ��߿�ʱ���������Ϊ0
//#define RECTANGLE_LENGTH	470.0f	//�����򳤶ȡ�Ҫ����ż��
#define RECTANGLE_LENGTH	80.0f	//�����򳤶ȡ�Ҫ����ż��
#define RECTANGLE_HEIGHT	40	//������߶ȡ�Ҫ����ż��

/********************��ӡ��������***********************/
#define CapStartX 960-100		//510
#define CapStartY 180+700
#define CapLength 200  //80
#define CapWidth 20
#define CapFrameColor Cyan
#define CapHeadWidth 16
#define CapHeadLength 10

/********************��ӡBUFF����*************************/
#define BUFF_TitleX 1920-200
#define BUFF_TitleY 820
#define BUFF_ALLX  1920-300
#define BUFF_StartY  780
#define BUFF_LineH 40

/********************����ߴ������ñ�***********************/
#define Judge_Car_Size_Line_Color			Yellow			//�ߵ���ɫ
#define Judge_Car_Size_Line_Width			2				//�߿�
#define Judge_Car_Size_BottomWidthMin		700				//����ߴ����²��ĳ��ȣ���λ�����ء���󳤶ȣ��������ܹ���С�ڴ˳ߴ���Բ��ܹ�
#define Judge_Car_Size_TopWidthMin			300				//����ߴ����ϲ��ĳ��ȣ���λ�����ء�
#define Judge_Car_Size_BottomWidthMax		950				//����ߴ����²��ĳ��ȣ���λ�����ء���󳤶ȣ���һ���ܹ�
#define Judge_Car_Size_TopWidthMax			600			//����ߴ����ϲ��ĳ��ȣ���λ�����ء�
#define Judge_Car_Size_LineHign				200				//����ߴ����ϲ������²��ĸ߶ȣ���λ������


/********************�м��������ñ�***********************/
#define Judge_DontDrawHorizontal	0		//��Ҫ�����ʱ�Ͱ��������Ϊ1		
#define Judge_Horizontal_Line_Color	Green	//ˮƽ�ߵ���ɫ
#define Judge_Horizontal_Line_Width	2	//�߿�
#define Judge_Horizontal_Line_Length	560	//�߳�
#define Judge_Vertical_Line_Height		10		//С��ֱ�߸߶�
#define Judge_DontDrawFigure	0		//��Ҫͼ��ʱ�Ͱ��������Ϊ1


/********************������׼�����ñ�----�ȼ�Ϊ1ʱ***********************/
#define Judge_Equ_Line_All_Color	Green	//�ߵ���ɫ
#define Judge_Equ_Line_All_Width	1	//�߿�
#define Judge_Equ_Line_Length_Vertical	20		//�����߳���
#define Judge_Equ_Line_ALL_Position_X	960		//���Ķ�λ������X

//�ȼ�Ϊ1ʱ���������µ�һ����
#define	Judge_Equ_Line_1_1_length_S	70		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_1_1_length_L	118		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_1_1_Position_Y	500		//���Ķ�λ������Y
#define Judge_Equ_Line_1_1_Label_float	5.2		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_1_1_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ȼ�Ϊ1ʱ���������µڶ�����
#define	Judge_Equ_Line_1_2_length_S	60		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_1_2_length_L	106		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_1_2_Position_Y	470		//���Ķ�λ������Y
#define Judge_Equ_Line_1_2_Label_float	2		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_1_2_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ�������
#define	Judge_Equ_Line_1_3_length_S	40		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_1_3_length_L	70		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_1_3_Position_Y	440		//���Ķ�λ������Y
#define Judge_Equ_Line_1_3_Label_float	3		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_1_3_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ȼ�Ϊ1ʱ���������µ��ĸ���
#define	Judge_Equ_Line_1_4_length_S	35		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_1_4_length_L	62		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_1_4_Position_Y	410		//���Ķ�λ������Y
#define Judge_Equ_Line_1_4_Label_float	4		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_1_4_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ȼ�Ϊ1ʱ���������µ������
#define	Judge_Equ_Line_1_5_length_S	25		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_1_5_length_L	44		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_1_5_Position_Y	380		//���Ķ�λ������Y
#define Judge_Equ_Line_1_5_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_1_5_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ȼ�Ϊ1ʱ���������µ�������
#define	Judge_Equ_Line_1_6_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_1_6_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_1_6_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_1_6_Label_float	6		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_1_6_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ȼ�Ϊ1ʱ���������µ��߸���
#define	Judge_Equ_Line_1_7_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_1_7_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_1_7_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_1_7_Label_float	7		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_1_7_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����


/********************������׼�����ñ�----�ȼ�Ϊ2ʱ***********************/

//�ȼ�Ϊ1ʱ���������µ�һ����
#define	Judge_Equ_Line_2_1_length_S	70		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_2_1_length_L	118		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_2_1_Position_Y	490		//���Ķ�λ������Y
#define Judge_Equ_Line_2_1_Label_float	5.2		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_2_1_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ȼ�Ϊ1ʱ���������µڶ�����
#define	Judge_Equ_Line_2_2_length_S	60		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_2_2_length_L	106		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_2_2_Position_Y	480		//���Ķ�λ������Y
#define Judge_Equ_Line_2_2_Label_float	2		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_2_2_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ȼ�Ϊ1ʱ���������µ�������
#define	Judge_Equ_Line_2_3_length_S	40		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_2_3_length_L	70		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_2_3_Position_Y	470		//���Ķ�λ������Y
#define Judge_Equ_Line_2_3_Label_float	3		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_2_3_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ��ĸ���
#define	Judge_Equ_Line_2_4_length_S	35		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_2_4_length_L	62		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_2_4_Position_Y	465		//���Ķ�λ������Y
#define Judge_Equ_Line_2_4_Label_float	4		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_2_4_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ������
#define	Judge_Equ_Line_2_5_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_2_5_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_2_5_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_2_5_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_2_5_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ�������
#define	Judge_Equ_Line_2_6_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_2_6_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_2_6_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_2_6_Label_float	6		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_2_6_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ��߸���
#define	Judge_Equ_Line_2_7_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_2_7_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_2_7_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_2_7_Label_float	7	//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_2_7_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
/********************������׼�����ñ�----�ȼ�Ϊ3ʱ***********************/

//�ȼ�Ϊ1ʱ���������µ�һ����
#define	Judge_Equ_Line_3_1_length_S	25		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_3_1_length_L	44		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_3_1_Position_Y	505		//���Ķ�λ������Y
#define Judge_Equ_Line_3_1_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_3_1_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µڶ�����
#define	Judge_Equ_Line_3_2_length_S	25		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_3_2_length_L	44		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_3_2_Position_Y	505		//���Ķ�λ������Y
#define Judge_Equ_Line_3_2_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_3_2_Length_Vertical	Judge_Equ_Line_Length_Vertical	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ�������
#define	Judge_Equ_Line_3_3_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_3_3_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_3_3_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_3_3_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_3_3_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ��ĸ���
#define	Judge_Equ_Line_3_4_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_3_4_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_3_4_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_3_4_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_3_4_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ������
#define	Judge_Equ_Line_3_5_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_3_5_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_3_5_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_3_5_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_3_5_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ�������
#define	Judge_Equ_Line_3_6_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_3_6_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_3_6_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_3_6_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_3_6_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����
//�ȼ�Ϊ1ʱ���������µ��߸���
#define	Judge_Equ_Line_3_7_length_S	0		//Сװ�װ��Ӧ�߳��ȣ���λ������
#define	Judge_Equ_Line_3_7_length_L	0		//��װ�װ��Ӧ�߳��ȣ���λ����
#define	Judge_Equ_Line_3_7_Position_Y	0		//���Ķ�λ������Y
#define Judge_Equ_Line_3_7_Label_float	5		//��λ���Աߵľ����ǩ����λ��m��С�������1λ
#define Judge_Equ_Line_3_7_Length_Vertical	0	//�����߳��ȣ�������Ҫ�����ʱ���ѳ��ȸ�Ϊ0����

//�ӻ�λ���ƶ�
#define X_move 0
#define Y_move 520




//////////////////////////////////////////////////////////////////////////////////////////
//****************************************************************************************
//########################################################################################
//�����ǽӿں������������ǽӿں������������ǽӿں������������ǽӿں������������ǽӿں��������Լ���Ҫʲô���ݾ�д��������ʲô����
//����





//�����ǽӿں������������ǽӿں�����,�����ǽӿں������������ǽӿں������������ǽӿں�����
//########################################################################################
//****************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////

/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :�ú����ǵ�һ����ֲ����ϵͳ������Ҫ�ܵ�һ�����Ժ���
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
 void Draw_DmaDoubleBuffTest(void);

 void Judge_Check(void);
 void Judge_Init(void);
/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judget.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :�ú����ǲ���ϵͳ��ʼ������
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
  

  
  

/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judget.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :�ú������ڲ����ֽ�����ʾ�������������ĺ�����Percent�ǰٷֱȣ�Voltage�ǵ�ѹֵ
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */

  
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//�����ǽӿں���

/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :Judge_GetRobotState�����ǻ�û�����״̬�����ĺ���
  * Description        :
  * Description        :
  * Description        :
  * Description        :
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */



int8_t PrintfYawPitchToScreen(float YawAngel,float PitchAngel);
int8_t PrintCircleColor(uint8_t WhichChannel,graphic_color_tpye_e NextColor);
int8_t PrintfToScreen(uint8_t WhichChannel,char *STR);

void Judge_LoopPer1MS(void);
void Draw_EqualHeightLine(uint8_t RobotLevel);

ext_game_robot_status_t* Judge_GetRobotState(void);

ext_power_heat_data_t* Judge_GetRobot_power_heat(void);


ext_game_robot_status_t* Judge_robot_status(void);

ext_power_heat_data_t* Judge_heat(void);

ext_game_robot_pos_t*	Judge_GetRobotPosition(void);

ext_robot_hurt_t* Judge_RobotHurt(void);

ext_game_state_t* Judge_Gametime(void);

ext_buff_t* Judge_BuffState(void);

//��ӡװ�װ���ת�ǶȺ�װ�װ���ɫ��10HZ���Ƕ�0-360����ʱ��Ϊ��
int8_t PrintArmorRotate(float Yaw,graphic_color_tpye_e Armor_1,graphic_color_tpye_e Armor_2,
						graphic_color_tpye_e Armor_3,graphic_color_tpye_e Armor_4);

/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :Judge_GetFlag�����ǻ�û�����ĳ��������û�и��µĺ���
  * Description        :����ֵ�Ǹò������¹��˶��ٴ�
  * Description        :����ֵ��Ҫ����ĳ���ṹ��
  * Description        :
  * Description        :
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
uint16_t Judge_GetFlag(frame_cmd_id_e WhichMsg);



/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :Judge_ClearFlagOf������������±�־λ�ĺ���
  * Description        :
  * Description        :����ֵ��Ҫ�������FLAG��ĳ���ṹ��
  * Description        :
  * Description        :
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */  
void Judge_ClearFlagOf(frame_cmd_id_e WhichMsg);



/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :void Judge_RobotToRobot(Robots_ID_e receiver, uint8_t *DataAddr);
  * Description        :�����˼�ͨ��
  * Description        :receiver�ǽ��շ�������ID���䶨��� Robots_ID_e
  * Description        :DataAddr��Ҫ���͵���Ϣ���ַ�����ʽ
  * Description        :
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
void Judge_RobotToRobot(Robots_ID_e receiver, uint8_t *DataAddr);


/**
  ******************************************************************************
  * Copyright (C), 2021 - , �Ĵ���ѧ���ս�� 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021��4��3��
  * Description        :void Judge_SelfControl(uint8_t *DataAddr)
  * Description        :�Զ����������������
  * Description        :DataAddr��Ҫ���͵���Ϣ���ַ�����ʽ
  * Description        :
  * Function List      :
  * History            :
        <author>    <version>    <time>    <desc>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 XXXX.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */

void Judge_SelfControl(uint8_t *DataAddr);
void Graph_DrawLineInit(graphic_data_struct_t *GraphData_t,uint8_t *GraphName,
						uint32_t Start_X,uint32_t Start_Y,uint32_t End_X,uint32_t 
						End_Y,uint32_t width,uint32_t Color);
void Draw_BuffStr(uint8_t *Name,uint16_t Buf_Color,uint16_t Position_X,uint16_t Position_Y,uint8_t* DisplayStr);
void Graph_DeleteLayer(int32_t Layer,uint8_t *Name);
void Draw_RobotState(void);
void Draw_DynamicNum(graphic_data_struct_t *GraphData_t,uint8_t *GraphName,
					uint32_t Start_X,uint32_t Start_Y,uint32_t start_angle,uint32_t Color, uint32_t Num);
void Draw_StaticNum(graphic_data_struct_t *GraphData_t,uint8_t *GraphName,
					uint32_t Start_X,uint32_t Start_Y,uint32_t start_angle,uint32_t Color, uint32_t Num);



/***************����ϵͳ��ʼ������****************/
void Judge_Init(void);



/***************��Ҫ��̬���µ�UI��ͼ����**********/
/***************�밴�����˳��ʹ�ú�����10ms����һ��**********/
/*
@FunctionName:
@Brief:��ͼʱ���־����
@Notice:��̬��ͼ����ʹ��
@LatestUpdate:2021.12.04
@Author:
*/
void Drawing_StateUpdate(void);
/*
@FunctionName:
@Brief:������ʾ
@Notice:
@LatestUpdate:
@Author:
*/
void Draw_Buff(void);

/*
@FunctionName:
@Brief:�����˵ȼ����˺�������ʾ
@Notice:
@LatestUpdate:
@Author:
*/
void Draw_RobotStatus(void);

/*
@FunctionName:
@Brief:��̨Ħ���ֺͲ��ֿ�����ʾ
@Notice:����Ħ���֣�����״̬��־��0�رգ�1��
@LatestUpdate:
@Author:
*/
void Draw_FricRammer(uint8_t fric_state,uint8_t rammer_state);

/*
@FunctionName:
@Brief:�����������ݸ���
@Notice:����Ϊ�������ݵ�ѹ��������0-100������� Draw_YawPitch()ͬʱ����
@LatestUpdate:
@Author:
*/
int8_t Draw_CapV(float Votlage2,float Percent);

/*
@FunctionName:
@Brief:yaw��pitch����ʾ
@Notice:����Ϊyaw,pitch�Ƕ�
@LatestUpdate:
@Author:
*/
void Draw_YawPitch(float yaw,float pitch);






/**************sentery �ӿں���������**************/
ext_shoot_data_t* Judge_shoot_data(void);
ext_bullet_remaining_t* JudgeRemainBulletNum(void);
ext_game_state_t Judge_Game_State(void);
ext_game_robot_HP_t Judge_Robot_HP(void);
ext_game_robot_status_t    Judge_Robot_State(void);

void sentryJudgeMSGPrint(void);
/************sentery �ӿں��������� END**************/

#ifdef __cplusplus
}
#endif	/*__cplusplus*/

#endif



