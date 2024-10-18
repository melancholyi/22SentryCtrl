/******************************
File name: TDT_Task\src\start_task.cpp
Description: 开始任务
function:
	——————————————————————————————————————————————————————————————————————————
	void Start_Task(void* pvParameters)
	——————————————————————————————————————————————————————————————————————————
Author: 肖银河
Version: 1.1.1.191112_alpha
Date: 19.11.12
History:
	——————————————————————————————————————————————————————————————————————————
	19.11.12 首次完成
	——————————————————————————————————————————————————————————————————————————
****************************  */
#include "StartTask.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/
#include "MyTask.h"

/*开始任务*/
extern TaskHandle_t StartTask_Handler;

/*呼吸灯任务*/
/*-摘要-*/ #define LED
	/*-优先-*/ #define LED_TASK_PRIO 1
	/*-堆栈-*/ #define LED_STK_SIZE 100
	/*-句柄-*/ TaskHandle_t LedTask_Handler;
/*-声明-*/ extern void Led_Task(void *pvParameters);

/*底盘电机任务*/
/*-摘要-*/ #define CHASSIS
/*-优先-*/ #define CHASSIS_TASK_PRIO 3
/*-堆栈-*/ #define CHASSIS_STK_SIZE 1024
/*-句柄-*/ TaskHandle_t ChassisTask_Handler;
/*-声明-*/ extern void Chassis_Task(void *pvParameters);

/*状态处理任务*/
/*-摘要-*/ #define STATE
/*-优先-*/ #define STATE_TASK_PRIO 4
/*-堆栈-*/ #define STATE_STK_SIZE 500
/*-句柄-*/ TaskHandle_t StateTask_Handler;
/*-声明-*/ extern void State_Task(void *pvParameters);

/*debug处理任务*/
/*-摘要-*/ #define DEBUG
/*-优先-*/ #define DEBUG_TASK_PRIO 1
/*-堆栈-*/ #define DEBUG_STK_SIZE 500
/*-句柄-*/ TaskHandle_t DebugTask_Handler;
/*-声明-*/ extern void Debug_Task(void *pvParameters);

/*CAN com任务*/
/*-摘要-*/ #define CANCOM
/*-优先-*/ #define CANCOM_TASK_PRIO 2
/*-堆栈-*/ #define CANCOM_STK_SIZE 1024
/*-句柄-*/ TaskHandle_t CancomTask_Handler;
/*-声明-*/ extern void Cancom_Task(void *pvParameters);

/**
  * @brief 开始任务
  * @note 用于创建任务，完事删了自己
  */
void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL(); //进入临界区
	//创建其他任务
	mTaskCreate(LED, Led);			//LED任务
	//mTaskCreate(CHASSIS,Chassis);	//chassis 	
	//mTaskCreate(STATE,State);		//state task
	mTaskCreate(DEBUG,Debug);		//debug task
	mTaskCreate(CANCOM,Cancom);		//can com task
	
	/*初始化*/
	boardALLInit();

	//删除开始任务
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL(); //退出临界区
}

