#include <vector>
#include "sys.h"
#include "cycle.h"
#include "MyTask.h"

#include "led.h"
#include "can.h"
#include "encoder.h"
#include "debug_usart.h"
#include "rng.h"
#include "led.h"
#include "judge.h"
#include "stateAction.h"
#include "ch2gm.h"
#include "optoeSwitch.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h"					//FreeRTOSʹ��	 
#include "task.h"
/**FreeRTOS*END***************/

/*��ʼ����*///���ڴ������񡢶��С���ʱ��
/*-ժҪ-*/	#define START
/*-����-*/	#define START_TASK_PRIO		1
/*-��ջ-*/	#define START_STK_SIZE 		100  
/*-���-*/	TaskHandle_t StartTask_Handler;
/*-����-*/	extern void Start_Task(void *pvParameters);


// void *pvPortMalloc( size_t xWantedSize );
// void * operator new(size_t size)
// {
// //	return pvPortMalloc(size);
// 	auto ptr = pvPortMalloc(size);
// 	memset(ptr,0,size);
// 	return ptr;
// }

// void operator delete(void *m)
// {
// 	vPortFree(m);
// }


/*ͨ������ĺ궨���� my_task.h ��*/
int main(void)
{
	
	/*�жϷ���*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    /*�δ�ʱ������*/
    sysTickInit();

	//can motor init
	CAN1_Init();
	CAN2_Init();
	Debug_Usart4_init(115200);
	LED led;
	RNG_Init();
	//Judge_Init();
	OE_SwitchInit();
	printf("init ok\n");

	
	/*������ʼ����*/
	mTaskCreate(START,Start);
	/*�������������*/
    vTaskStartScheduler();
}
















