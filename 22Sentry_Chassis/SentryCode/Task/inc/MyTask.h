#ifndef __MY_TASK_H__
#define __MY_TASK_H__

#include "sys.h"

#ifdef __cplusplus
 extern "C" {
#endif	/*__cplusplus*/


/*??�������񴴽����������ڴ������񣬲���c��capitalȫ��д��l��lowercase����ĸ��??*/
//	 	mTaskCreate(START,Start);
#define mTaskCreate(c,l)    xTaskCreate((TaskFunction_t )l##_Task,		\
										(const char*    )#l,		\
										(uint16_t       )c##_STK_SIZE,	\
										(void*          )NULL,			\
										(UBaseType_t    )c##_TASK_PRIO,	\
										(TaskHandle_t*  )&l##Task_Handler)
/*��ȡ������ϸ��Ϣ*/
//�����÷���???�ö���??���꣬����չ������		
#define mTaskGetInfo(x)	_mTaskGetInfo(x)
#define _mTaskGetInfo(x)	vTaskGetInfo((TaskHandle_t)x##Task_Handler,	\
							(TaskStatus_t*)&x##TaskStatus,	\
							(BaseType_t)pdTRUE,			\
							(eTaskState)eInvalid);		

//���ڵδ�ʱ����????�ģ����������Ժ꣬��ms????Ϊ��??
//����ʹ��pdMS_TO_TICKS()��ʱ��ת??Ϊ��??
#ifndef MPRE
	#error Please define MPRE at first.							
#endif
//������ʱ����
#define mTaskDelay(x) vTaskDelay(x*MPRE)
//��ms????Ϊ��??	
#define MS(x) x*MPRE
//��ʱ??
#define mTimerCreate(a,b,c,d,e) xTimerCreate(a,b*MPRE,c,d,e)
	
/////////////////DBUG����??

/*�꣺���ڴ�ӡ��Ϣ��lcd������y��???��0-7��a��???��ָ��*/
#define LCD_P(y,a)   LCD_Print(0, (y)*8,(u8*)a,TYPE6X8,TYPE6X8);
/*�꣺���sprintf��������ӡ��??������y��???��??...��???�÷���printf��ͬ*/
#define LCD(ID,y,...) 	OLED_CS(ID);sprintf(publicChar,__VA_ARGS__); LCD_P(y,publicChar);OLED_CS(!ID);


//LCD��ʾ����ջ��ȸ�ˮλ?? ������a����ժ???��x��ʾ��x���꣬y��ʾ��???��0-7						
#define Dbug_LCD_HWM(a,x,y)		extern TaskHandle_t a##Task_Handler;	\
								char a##HWM[20]="";				\
								sprintf(a##HWM,#a" HWM:%ld",uxTaskGetStackHighWaterMark(a##Task_Handler));	\
								LCD_Print(x, y*8,(u8*)a##HWM,TYPE6X8,TYPE6X8);

/*���ڵ���??*/
//�������ջ��ȸ�ˮλ??
#define Dbug_Printf_HWM(x) 				extern TaskHandle_t x##Task_Handler;	\
										printf(#x"ˮλ??%ld\r\n",uxTaskGetStackHighWaterMark(x##Task_Handler));
										



#ifdef __cplusplus
}
#endif	/*__cplusplus*/
#endif
