/******************************
start task
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

/*�����ǽ�������*/
/*-ժҪ-*/ #define IMU
/*-����-*/ #define IMU_TASK_PRIO 4
/*-��ջ-*/ #define IMU_STK_SIZE 1000
/*-���-*/ TaskHandle_t ImuTask_Handler;
/*-����-*/ extern void Imu_Task(void *pvParameters);

/*��̨��������*/
/*-ժҪ-*/ #define GIMBAL
/*-����-*/ #define GIMBAL_TASK_PRIO 5
/*-��ջ-*/ #define GIMBAL_STK_SIZE 1000
/*-���-*/ TaskHandle_t GimbalTask_Handler;
/*-����-*/ extern void Gimbal_Task(void *pvParameters);

/*״̬��������*/
/*-ժҪ-*/ #define STATE
/*-����-*/ #define STATE_TASK_PRIO 3
/*-��ջ-*/ #define STATE_STK_SIZE 500
/*-���-*/ TaskHandle_t StateTask_Handler;
/*-����-*/ extern void State_Task(void *pvParameters);

/*debug��������*/
/*-ժҪ-*/ #define DEBUG
/*-����-*/ #define DEBUG_TASK_PRIO 2
/*-��ջ-*/ #define DEBUG_STK_SIZE 500
/*-���-*/ TaskHandle_t DebugTask_Handler;
/*-����-*/ extern void Debug_Task(void *pvParameters);

/*cancom����*/
/*-ժҪ-*/ #define CANCOM
/*-����-*/ #define CANCOM_TASK_PRIO 1
/*-��ջ-*/ #define CANCOM_STK_SIZE 1024
/*-���-*/ TaskHandle_t CancomTask_Handler;
/*-����-*/ extern void Cancom_Task(void *pvParameters);

// int8_t creatResult[10] = {-1};
// u8 taskNum = 0; //����������

/**
  * @brief ��ʼ����
  * @note ���ڴ�����������ɾ���Լ�
  */
void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL(); //�����ٽ���
	//������������
	mTaskCreate(LED, Led);			//LED����
	mTaskCreate(IMU,Imu);			//����������
	mTaskCreate(GIMBAL,Gimbal);		//gimbal 	
	// mTaskCreate(STATE,State);			//state task
	mTaskCreate(DEBUG,Debug);		//debug print task
	#if Gimbal_TOP_DEFINE  //TBSYCY
	// mTaskCreate(CANCOM,Cancom);		//can com task
	#endif
	/*��ʼ��*/
	boardALLInit();

	//ɾ����ʼ����
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL(); //�˳��ٽ���
}

