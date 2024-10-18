#ifndef __MY_TASK_H__
#define __MY_TASK_H__

#include "sys.h"

#ifdef __cplusplus
 extern "C" {
#endif	/*__cplusplus*/


/*??定义任务创建函数，用于创建任务，参数c：capital全大写；l：lowercase首字母大??*/
//	 	mTaskCreate(START,Start);
#define mTaskCreate(c,l)    xTaskCreate((TaskFunction_t )l##_Task,		\
										(const char*    )#l,		\
										(uint16_t       )c##_STK_SIZE,	\
										(void*          )NULL,			\
										(UBaseType_t    )c##_TASK_PRIO,	\
										(TaskHandle_t*  )&l##Task_Handler)
/*获取任务详细信息*/
//特殊用法，???置二级??换宏，用于展开参数		
#define mTaskGetInfo(x)	_mTaskGetInfo(x)
#define _mTaskGetInfo(x)	vTaskGetInfo((TaskHandle_t)x##Task_Handler,	\
							(TaskStatus_t*)&x##TaskStatus,	\
							(BaseType_t)pdTRUE,			\
							(eTaskState)eInvalid);		

//由于滴答定时器中????改，定义如下以宏，将ms????为节??
//或者使用pdMS_TO_TICKS()将时间转??为节??
#ifndef MPRE
	#error Please define MPRE at first.							
#endif
//任务延时函数
#define mTaskDelay(x) vTaskDelay(x*MPRE)
//将ms????为节??	
#define MS(x) x*MPRE
//定时??
#define mTimerCreate(a,b,c,d,e) xTimerCreate(a,b*MPRE,c,d,e)
	
/////////////////DBUG调试??

/*宏：用于打印信息到lcd，参数y：???数0-7，a字???串指针*/
#define LCD_P(y,a)   LCD_Print(0, (y)*8,(u8*)a,TYPE6X8,TYPE6X8);
/*宏：配合sprintf函数，打印信??。参数y：???数??...此???用法与printf相同*/
#define LCD(ID,y,...) 	OLED_CS(ID);sprintf(publicChar,__VA_ARGS__); LCD_P(y,publicChar);OLED_CS(!ID);


//LCD显示任务栈深度高水位?? 参数：a任务摘???，x显示的x坐标，y显示的???数0-7						
#define Dbug_LCD_HWM(a,x,y)		extern TaskHandle_t a##Task_Handler;	\
								char a##HWM[20]="";				\
								sprintf(a##HWM,#a" HWM:%ld",uxTaskGetStackHighWaterMark(a##Task_Handler));	\
								LCD_Print(x, y*8,(u8*)a##HWM,TYPE6X8,TYPE6X8);

/*串口调试??*/
//输出任务栈深度高水位??
#define Dbug_Printf_HWM(x) 				extern TaskHandle_t x##Task_Handler;	\
										printf(#x"水位??%ld\r\n",uxTaskGetStackHighWaterMark(x##Task_Handler));
										



#ifdef __cplusplus
}
#endif	/*__cplusplus*/
#endif
