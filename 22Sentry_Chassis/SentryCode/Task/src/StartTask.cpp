/******************************
File name: TDT_Task\src\start_task.cpp
Description: ��ʼ����
function:
	����������������������������������������������������������������������������������������������������������������������������������������������������
	void Start_Task(void* pvParameters)
	����������������������������������������������������������������������������������������������������������������������������������������������������
Author: Ф����
Version: 1.1.1.191112_alpha
Date: 19.11.12
History:
	����������������������������������������������������������������������������������������������������������������������������������������������������
	19.11.12 �״����
	����������������������������������������������������������������������������������������������������������������������������������������������������
****************************  */
#include "StartTask.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOSʹ��
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/
#include "MyTask.h"

/*��ʼ����*/
extern TaskHandle_t StartTask_Handler;

/*����������*/
/*-ժҪ-*/ #define LED
	/*-����-*/ #define LED_TASK_PRIO 1
	/*-��ջ-*/ #define LED_STK_SIZE 100
	/*-���-*/ TaskHandle_t LedTask_Handler;
/*-����-*/ extern void Led_Task(void *pvParameters);

/*���̵������*/
/*-ժҪ-*/ #define CHASSIS
/*-����-*/ #define CHASSIS_TASK_PRIO 3
/*-��ջ-*/ #define CHASSIS_STK_SIZE 1024
/*-���-*/ TaskHandle_t ChassisTask_Handler;
/*-����-*/ extern void Chassis_Task(void *pvParameters);

/*״̬��������*/
/*-ժҪ-*/ #define STATE
/*-����-*/ #define STATE_TASK_PRIO 4
/*-��ջ-*/ #define STATE_STK_SIZE 500
/*-���-*/ TaskHandle_t StateTask_Handler;
/*-����-*/ extern void State_Task(void *pvParameters);

/*debug��������*/
/*-ժҪ-*/ #define DEBUG
/*-����-*/ #define DEBUG_TASK_PRIO 1
/*-��ջ-*/ #define DEBUG_STK_SIZE 500
/*-���-*/ TaskHandle_t DebugTask_Handler;
/*-����-*/ extern void Debug_Task(void *pvParameters);

/*CAN com����*/
/*-ժҪ-*/ #define CANCOM
/*-����-*/ #define CANCOM_TASK_PRIO 2
/*-��ջ-*/ #define CANCOM_STK_SIZE 1024
/*-���-*/ TaskHandle_t CancomTask_Handler;
/*-����-*/ extern void Cancom_Task(void *pvParameters);

/**
  * @brief ��ʼ����
  * @note ���ڴ�����������ɾ���Լ�
  */
void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL(); //�����ٽ���
	//������������
	mTaskCreate(LED, Led);			//LED����
	//mTaskCreate(CHASSIS,Chassis);	//chassis 	
	//mTaskCreate(STATE,State);		//state task
	mTaskCreate(DEBUG,Debug);		//debug task
	mTaskCreate(CANCOM,Cancom);		//can com task
	
	/*��ʼ��*/
	boardALLInit();

	//ɾ����ʼ����
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL(); //�˳��ٽ���
}

