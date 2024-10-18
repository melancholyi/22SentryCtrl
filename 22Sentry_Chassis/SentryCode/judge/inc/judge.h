/**
  ******************************************************************************
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :看配置表
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
  * 更新日志：
  * 2021年1月10日：查找到judge.c的第512行，USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);//开启串口DMA
  * 应改为USART_DMACmd(Judge_UseUART_Typedef_Name,USART_DMAReq_Tx,ENABLE);
  *
  *	2021年4月3日：收到了新版的裁判系统，开始移植。
  *2021.12.05
  */

#ifndef __JUDGE_H__
#define __JUDGE_H__

#include "judgeStruct.h"
#include "CRCCheck.h"

#ifdef __cplusplus
extern "C"{
#endif	/*__cplusplus*/
/********************串口和DMA引脚配置表***********************/

//引脚相关，更改RX,TX引脚为裁判系统串口IO口端口和引脚
#define Judge_UseGPIO_GPIO_PeriphClock RCC_AHB1Periph_GPIOB		
#define Judge_UseGPIO_GPIO_Typedef_Name	GPIOB
#define Judge_UseGPIO_GPIO_PINSource_TX	GPIO_PinSource10		
#define Judge_UseGPIO_GPIO_GPIO_Pin_TX	GPIO_Pin_10
#define Judge_UseGPIO_GPIO_PINSource_RX	GPIO_PinSource11
#define Judge_UseGPIO_GPIO_GPIO_Pin_RX	GPIO_Pin_11

//串口相关，更改串口为所使用串口数字
#define Judge_UseUART_PeriphClock	RCC_APB1Periph_USART3
#define Judge_UseUART_GPIOAF_Name GPIO_AF_USART3
#define Judge_UseUART_Typedef_Name	USART3
#define Judge_UseUART_IRQn	USART3_IRQn
#define Judge_UseUART_IRQHandler	USART3_IRQHandler

//函数名相关，更改数字为当前串口
#define Judge_UseUART_Function_Name	USART3_Init
#define Judge_UseUART_SendChar	USART3_SendChar


//DMA配置相关
//RX的DMA
#define Judge_UseDMA_PeriphClock_RX	RCC_AHB1Periph_DMA1				//数字改为当前串口RX的DMA数字
#define Judge_UseDMA_IRQChannel_RX	DMA1_Stream1_IRQn				//数字改为对应DMA和数据流数字
#define Judge_UseDMA_Stream_Typedef_Name_RX	DMA1_Stream1			//数字改为数据流数字
#define Judge_UseDMA_ClearIT_RX	DMA_IT_TCIF1						//数字改为数据流数字
#define Judge_UseDMA_Channel_RX	DMA_Channel_4						//数字改为通道数字
#define Judge_UseDMA_ClearFlag	DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1		//数字改为通道数字
//函数
#define Judge_UseDMA_IRQHandler_RX			DMA1_Stream1_IRQHandler	//数字改为对应DMA和数据流数字
//TX的DMA
#define Judge_UseDMA_PeriphClock_TX			RCC_AHB1Periph_DMA1		//数字改为当前串口TX的DMA数字
#define Judge_UseDMA_IRQChannel_TX			DMA1_Stream3_IRQn		//数字改为对应DMA和数据流数字
#define Judge_UseDMA_Stream_Typedef_Name_TX	DMA1_Stream3			//数字改为对应DMA和数据流数字
#define Judge_UseDMA_ClearIT_TX				DMA_IT_TCIF3			//数字改为数据流数字
#define Judge_UseDMA_Channel_TX				DMA_Channel_4  			//数字改为通道数字
//函数
#define Judge_UseDMA_IRQHandler_TX	DMA1_Stream3_IRQHandler			//数字改为对应DMA和数据流数字




///////////////////////////////////////////////////////////////配置表
/********************4路装甲板配置表，如果使用，一定要把GRAPH_USE_YAW_OR_ARMOR定义为1***********************/
#define JUDGE_GRAPH_DISTANCE	200.0f		//装甲板到旋转中心距离
#define JUDGE_GRAPH_ROTATE_X	960.0f		//旋转中心X坐标
#define JUDGE_GRAPH_ROTATE_Y	540.0f		//旋转中心Y坐标
#define JUDGE_GRAPH_BOARD_LENGTH	50.0f	//装甲板长度
#define JUDGE_GRAPH_BOARD_WIDTH	20.0f		//装甲板宽度


/********************7路字符串通道配置表***********************/
#define CHAR_SIZE_CH1	15
#define WIDTH_CH1      2
#define COLOR_CH1      Yellow
#define POSITION_X_CH1  50  
#define POSITION_Y_CH1  880
#define SPACE_Y_CHANNEL	30

/********************7路图形通道配置表***********************/
//共性配置
#define GRAPH_CHANNEL_ALL_RADIUS	8	//圆半径
#define GRAPH_CHANNEL_ALL_LENGTH	20	//矩形长度
#define GRAPH_CHANNEL_STR_COLOR_ALL	Green
#define GRAPH_CHANNEL_STR_LARGE_ALL	20		//字符串字体大小
#define GRAPH_CHANNEL_STR_WIDTH_ALL	2		//字符串线宽

//个性配置
#define GRAPH_CHANNEL_DELT_POSITION  70 //图形间距

#define GRAPH_USE_THIS_GRAPH_8_1		1		//把这个设置为1表示使用这个图形，为0不使用这个图形
#define GRAPH_CHANNEL_8_1_POSITION_X	750
#define GRAPH_CHANNEL_8_1_POSITION_Y	1080-300
#define GRAPH_CHANNEL_8_1_WIDTH			20	//线条宽度
#define GRAPH_CHANNEL_STR_8_1_POSITION_X	GRAPH_CHANNEL_8_1_POSITION_X		//色块1说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_1_POSITION_Y	GRAPH_CHANNEL_8_1_POSITION_Y - 20	//色块1说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_1	"F"	//色块1的提示说明字符串

#define GRAPH_USE_THIS_GRAPH_8_2		1		//把这个设置为1表示使用这个图形，为0不使用这个图形
#define GRAPH_CHANNEL_8_2_POSITION_X	GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION
#define GRAPH_CHANNEL_8_2_POSITION_Y	1080-300
#define GRAPH_CHANNEL_8_2_WIDTH			20	//线条宽度
#define GRAPH_CHANNEL_STR_8_2_POSITION_X	GRAPH_CHANNEL_8_2_POSITION_X		//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_2_POSITION_Y	GRAPH_CHANNEL_8_2_POSITION_Y - 20 //色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_2	"D"	//色块的提示说明字符串

#define GRAPH_USE_THIS_GRAPH_8_3		1		//把这个设置为1表示使用这个图形，为0不使用这个图形
#define GRAPH_CHANNEL_8_3_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*2)
#define GRAPH_CHANNEL_8_3_POSITION_Y	1080-300
#define GRAPH_CHANNEL_8_3_WIDTH			20	//线条宽度。
#define GRAPH_CHANNEL_STR_8_3_POSITION_X	GRAPH_CHANNEL_8_3_POSITION_X		//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_3_POSITION_Y	GRAPH_CHANNEL_8_3_POSITION_Y - 20	//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_3	"C"	//色块的提示说明字符串

#define GRAPH_USE_THIS_GRAPH_8_4		0		//把这个设置为1表示使用这个图形，为0不使用这个图形
#define GRAPH_CHANNEL_8_4_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*3)
#define GRAPH_CHANNEL_8_4_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_4_WIDTH			20	//线条宽度。
#define GRAPH_CHANNEL_STR_8_4_POSITION_X	GRAPH_CHANNEL_8_4_POSITION_X		//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_4_POSITION_Y	GRAPH_CHANNEL_8_4_POSITION_Y	//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_4	"GRAPH_1"	//色块的提示说明字符串

#define GRAPH_USE_THIS_GRAPH_8_5		0		//把这个设置为1表示使用这个图形，为0不使用这个图形
#define GRAPH_CHANNEL_8_5_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*4)
#define GRAPH_CHANNEL_8_5_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_5_WIDTH			20	//线条宽度。
#define GRAPH_CHANNEL_STR_8_5_POSITION_X	GRAPH_CHANNEL_8_5_POSITION_X	//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_5_POSITION_Y	GRAPH_CHANNEL_8_5_POSITION_Y	//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_5	"GRAPH_1"	//色块的提示说明字符串

#define GRAPH_USE_THIS_GRAPH_8_6		0		//把这个设置为1表示使用这个图形，为0不使用这个图形
#define GRAPH_CHANNEL_8_6_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*5)
#define GRAPH_CHANNEL_8_6_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_6_WIDTH			20	//线条宽度。
#define GRAPH_CHANNEL_STR_8_6_POSITION_X	GRAPH_CHANNEL_8_6_POSITION_X		//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_6_POSITION_Y	GRAPH_CHANNEL_8_6_POSITION_Y	//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_6	"GRAPH_1"	//色块的提示说明字符串

#define GRAPH_USE_THIS_GRAPH_8_7		0		//把这个设置为1表示使用这个图形，为0不使用这个图形
#define GRAPH_CHANNEL_8_7_POSITION_X	(GRAPH_CHANNEL_8_1_POSITION_X+GRAPH_CHANNEL_DELT_POSITION*6)
#define GRAPH_CHANNEL_8_7_POSITION_Y	1080-270
#define GRAPH_CHANNEL_8_7_WIDTH			20	//线条宽度。
#define GRAPH_CHANNEL_STR_8_7_POSITION_X	GRAPH_CHANNEL_8_7_POSITION_X		//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_7_POSITION_Y	GRAPH_CHANNEL_8_7_POSITION_Y	//色块说明字符的X坐标
#define GRAPH_CHANNEL_STR_8_7	"GRAPH_1"	//色块的提示说明字符串


/********************YAW轴的PITCH轴配置表***********************/
#define GRAPH_USE_YAW_OR_ARMOR	0	//0的时候使用YAW轴，1时不使用YAW轴，使用装甲板旋转

#define CHAR_SIZE_YAW_PIT	16
#define WIDTH_YAW_PIT      2
#define COLOR_YAW_PIT      Green
#define POSITION_X__YAW_PIT  580  
#define POSITION_Y__YAW_PIT  540



/********************打印超级电容配置表***********************/
#define CHAR_SIZE_VOLTAGE_2	14	//电压字符大小
#define WIDTH_VOLTAGE_2      2	//电压字符线宽
#define COLOR_VOLTAGE_2      Green	//电压字符颜色
#define VOLTAGE_DONT_USE	0		//如果把这个定义为1，就是不要电容外面显示的电压
#define POSITION_X_CAP  505
#define POSITION_Y_CAP  180
#define RECTANGLE_WIDTH	2	//电池外边框厚度，必须要为偶数，不然会干涉不好看。不想要外边框时把这个定义为0
//#define RECTANGLE_LENGTH	470.0f	//电池外框长度。要求是偶数
#define RECTANGLE_LENGTH	80.0f	//电池外框长度。要求是偶数
#define RECTANGLE_HEIGHT	40	//电池外框高度。要求是偶数

/********************打印超级电容***********************/
#define CapStartX 960-100		//510
#define CapStartY 180+700
#define CapLength 200  //80
#define CapWidth 20
#define CapFrameColor Cyan
#define CapHeadWidth 16
#define CapHeadLength 10

/********************打印BUFF数据*************************/
#define BUFF_TitleX 1920-200
#define BUFF_TitleY 820
#define BUFF_ALLX  1920-300
#define BUFF_StartY  780
#define BUFF_LineH 40

/********************车身尺寸线配置表***********************/
#define Judge_Car_Size_Line_Color			Yellow			//线的颜色
#define Judge_Car_Size_Line_Width			2				//线宽
#define Judge_Car_Size_BottomWidthMin		700				//车身尺寸线下部的长度，单位是像素。最大长度，车可能能过，小于此尺寸绝对不能过
#define Judge_Car_Size_TopWidthMin			300				//车身尺寸线上部的长度，单位是像素。
#define Judge_Car_Size_BottomWidthMax		950				//车身尺寸线下部的长度，单位是像素。最大长度，车一定能过
#define Judge_Car_Size_TopWidthMax			600			//车身尺寸线上部的长度，单位是像素。
#define Judge_Car_Size_LineHign				200				//车身尺寸线上部距离下部的高度，单位是像素


/********************中间大横线配置表***********************/
#define Judge_DontDrawHorizontal	0		//不要大横线时就把这个定义为1		
#define Judge_Horizontal_Line_Color	Green	//水平线的颜色
#define Judge_Horizontal_Line_Width	2	//线宽
#define Judge_Horizontal_Line_Length	560	//线长
#define Judge_Vertical_Line_Height		10		//小竖直线高度
#define Judge_DontDrawFigure	0		//不要图像时就把这个定义为1


/********************辅助瞄准线配置表----等级为1时***********************/
#define Judge_Equ_Line_All_Color	Green	//线的颜色
#define Judge_Equ_Line_All_Width	1	//线宽
#define Judge_Equ_Line_Length_Vertical	20		//短竖线长度
#define Judge_Equ_Line_ALL_Position_X	960		//中心定位点坐标X

//等级为1时，从上往下第一根线
#define	Judge_Equ_Line_1_1_length_S	70		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_1_1_length_L	118		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_1_1_Position_Y	500		//中心定位点坐标Y
#define Judge_Equ_Line_1_1_Label_float	5.2		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_1_1_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可

//等级为1时，从上往下第二根线
#define	Judge_Equ_Line_1_2_length_S	60		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_1_2_length_L	106		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_1_2_Position_Y	470		//中心定位点坐标Y
#define Judge_Equ_Line_1_2_Label_float	2		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_1_2_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第三根线
#define	Judge_Equ_Line_1_3_length_S	40		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_1_3_length_L	70		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_1_3_Position_Y	440		//中心定位点坐标Y
#define Judge_Equ_Line_1_3_Label_float	3		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_1_3_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可

//等级为1时，从上往下第四根线
#define	Judge_Equ_Line_1_4_length_S	35		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_1_4_length_L	62		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_1_4_Position_Y	410		//中心定位点坐标Y
#define Judge_Equ_Line_1_4_Label_float	4		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_1_4_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可

//等级为1时，从上往下第五根线
#define	Judge_Equ_Line_1_5_length_S	25		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_1_5_length_L	44		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_1_5_Position_Y	380		//中心定位点坐标Y
#define Judge_Equ_Line_1_5_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_1_5_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可

//等级为1时，从上往下第六根线
#define	Judge_Equ_Line_1_6_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_1_6_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_1_6_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_1_6_Label_float	6		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_1_6_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可

//等级为1时，从上往下第七根线
#define	Judge_Equ_Line_1_7_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_1_7_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_1_7_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_1_7_Label_float	7		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_1_7_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可


/********************辅助瞄准线配置表----等级为2时***********************/

//等级为1时，从上往下第一根线
#define	Judge_Equ_Line_2_1_length_S	70		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_2_1_length_L	118		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_2_1_Position_Y	490		//中心定位点坐标Y
#define Judge_Equ_Line_2_1_Label_float	5.2		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_2_1_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可

//等级为1时，从上往下第二根线
#define	Judge_Equ_Line_2_2_length_S	60		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_2_2_length_L	106		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_2_2_Position_Y	480		//中心定位点坐标Y
#define Judge_Equ_Line_2_2_Label_float	2		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_2_2_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可

//等级为1时，从上往下第三根线
#define	Judge_Equ_Line_2_3_length_S	40		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_2_3_length_L	70		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_2_3_Position_Y	470		//中心定位点坐标Y
#define Judge_Equ_Line_2_3_Label_float	3		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_2_3_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第四根线
#define	Judge_Equ_Line_2_4_length_S	35		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_2_4_length_L	62		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_2_4_Position_Y	465		//中心定位点坐标Y
#define Judge_Equ_Line_2_4_Label_float	4		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_2_4_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第五根线
#define	Judge_Equ_Line_2_5_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_2_5_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_2_5_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_2_5_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_2_5_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第六根线
#define	Judge_Equ_Line_2_6_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_2_6_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_2_6_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_2_6_Label_float	6		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_2_6_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第七根线
#define	Judge_Equ_Line_2_7_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_2_7_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_2_7_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_2_7_Label_float	7	//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_2_7_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可
/********************辅助瞄准线配置表----等级为3时***********************/

//等级为1时，从上往下第一根线
#define	Judge_Equ_Line_3_1_length_S	25		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_3_1_length_L	44		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_3_1_Position_Y	505		//中心定位点坐标Y
#define Judge_Equ_Line_3_1_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_3_1_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第二根线
#define	Judge_Equ_Line_3_2_length_S	25		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_3_2_length_L	44		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_3_2_Position_Y	505		//中心定位点坐标Y
#define Judge_Equ_Line_3_2_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_3_2_Length_Vertical	Judge_Equ_Line_Length_Vertical	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第三根线
#define	Judge_Equ_Line_3_3_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_3_3_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_3_3_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_3_3_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_3_3_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第四根线
#define	Judge_Equ_Line_3_4_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_3_4_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_3_4_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_3_4_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_3_4_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第五根线
#define	Judge_Equ_Line_3_5_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_3_5_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_3_5_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_3_5_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_3_5_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第六根线
#define	Judge_Equ_Line_3_6_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_3_6_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_3_6_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_3_6_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_3_6_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可
//等级为1时，从上往下第七根线
#define	Judge_Equ_Line_3_7_length_S	0		//小装甲板对应线长度，单位是像素
#define	Judge_Equ_Line_3_7_length_L	0		//大装甲板对应线长度，单位像素
#define	Judge_Equ_Line_3_7_Position_Y	0		//中心定位点坐标Y
#define Judge_Equ_Line_3_7_Label_float	5		//定位线旁边的距离标签，单位是m，小数点后面1位
#define Judge_Equ_Line_3_7_Length_Vertical	0	//短竖线长度，当不想要这根线时，把长度改为0即可

//队徽位置移动
#define X_move 0
#define Y_move 520




//////////////////////////////////////////////////////////////////////////////////////////
//****************************************************************************************
//########################################################################################
//下面是接口函数区。下面是接口函数区。下面是接口函数区。下面是接口函数区。下面是接口函数区。自己需要什么数据就写函数返回什么数据
//例如





//上面是接口函数区。上面是接口函数区,上面是接口函数区，上面是接口函数区，上面是接口函数区
//########################################################################################
//****************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////

/**
  ******************************************************************************
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :该函数是第一次移植裁判系统后，首先要跑的一个测试函数
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
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judget.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :该函数是裁判系统初始化函数
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
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judget.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :该函数是在操作手界面显示超级电容容量的函数，Percent是百分比，Voltage是电压值
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
//下面是接口函数

/**
  ******************************************************************************
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :Judge_GetRobotState函数是获得机器人状态参数的函数
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

//打印装甲板旋转角度和装甲板颜色，10HZ。角度0-360，逆时针为正
int8_t PrintArmorRotate(float Yaw,graphic_color_tpye_e Armor_1,graphic_color_tpye_e Armor_2,
						graphic_color_tpye_e Armor_3,graphic_color_tpye_e Armor_4);

/**
  ******************************************************************************
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :Judge_GetFlag函数是获得机器人某个参数有没有更新的函数
  * Description        :返回值是该参数更新过了多少次
  * Description        :输入值是要看的某个结构体
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
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :Judge_ClearFlagOf函数是清除更新标志位的函数
  * Description        :
  * Description        :输入值是要清除更新FLAG的某个结构体
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
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :void Judge_RobotToRobot(Robots_ID_e receiver, uint8_t *DataAddr);
  * Description        :机器人间通信
  * Description        :receiver是接收方机器人ID，其定义见 Robots_ID_e
  * Description        :DataAddr是要发送的信息。字符串格式
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
  * Copyright (C), 2021 - , 四川大学火锅战队 
  * File Name          :Judge.h
  * Author             :wcc
  * Version            :V1.4
  * date               :2021年4月3日
  * Description        :void Judge_SelfControl(uint8_t *DataAddr)
  * Description        :自定义控制器发送数据
  * Description        :DataAddr是要发送的信息。字符串格式
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



/***************裁判系统初始化函数****************/
void Judge_Init(void);



/***************需要动态更新的UI画图函数**********/
/***************请按下面的顺序使用函数，10ms更新一次**********/
/*
@FunctionName:
@Brief:画图时间标志更新
@Notice:动态画图必须使用
@LatestUpdate:2021.12.04
@Author:
*/
void Drawing_StateUpdate(void);
/*
@FunctionName:
@Brief:增益显示
@Notice:
@LatestUpdate:
@Author:
*/
void Draw_Buff(void);

/*
@FunctionName:
@Brief:机器人等级和伤害类型显示
@Notice:
@LatestUpdate:
@Author:
*/
void Draw_RobotStatus(void);

/*
@FunctionName:
@Brief:云台摩擦轮和拨轮开合显示
@Notice:输入摩擦轮，拨轮状态标志，0关闭，1打开
@LatestUpdate:
@Author:
*/
void Draw_FricRammer(uint8_t fric_state,uint8_t rammer_state);

/*
@FunctionName:
@Brief:超级电容数据更新
@Notice:输入为超级电容电压，电量（0-100），须和 Draw_YawPitch()同时更新
@LatestUpdate:
@Author:
*/
int8_t Draw_CapV(float Votlage2,float Percent);

/*
@FunctionName:
@Brief:yaw，pitch轴显示
@Notice:输入为yaw,pitch角度
@LatestUpdate:
@Author:
*/
void Draw_YawPitch(float yaw,float pitch);






/**************sentery 接口函数声明区**************/
ext_shoot_data_t* Judge_shoot_data(void);
ext_bullet_remaining_t* JudgeRemainBulletNum(void);
ext_game_state_t Judge_Game_State(void);
ext_game_robot_HP_t Judge_Robot_HP(void);
ext_game_robot_status_t    Judge_Robot_State(void);

void sentryJudgeMSGPrint(void);
/************sentery 接口函数声明区 END**************/

#ifdef __cplusplus
}
#endif	/*__cplusplus*/

#endif



