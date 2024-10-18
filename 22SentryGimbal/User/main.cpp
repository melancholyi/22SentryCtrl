#include <vector>
#include "sys.h"
#include "cycle.h"
#include "MyTask.h"
#include "VisionCom.h"
#include "spi.h"
#include "BMI088driver.h"
#include "imu.h"

#include "led.h"
#include "can.h"
#include "encoder.h"
#include "GimabalTask.h"
#include "dbus.h"
#include "usart6.h"
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


void *pvPortMalloc( size_t xWantedSize );
void * operator new(size_t size)
{
//	return pvPortMalloc(size);
	auto ptr = pvPortMalloc(size);
	memset(ptr,0,size);
	return ptr;
}

void operator delete(void *m)
{
	vPortFree(m);
}


/*ͨ������ĺ궨���� my_task.h ��*/
int main(void)
{
	
	/*�жϷ���*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    /*�δ�ʱ������*/
    sysTickInit();
	VISION_COM_Usart_Init(1152000);
	USART6_Init(115200);
	//can motor init
	CAN1_Init();
	CAN2_Init();
	
	//imu init
	SPI1_Init();		// drive imu
	while(BMI088_init()){  
		printf("error\r\n");
    	delayMs(1000);
	} // bmi088 <==> imu
	delayMs(500);
	for(uint16_t i =0 ;i<=3334; i++)	{
		IMUOffset_Init();
		delayMs(1);
	} 

	#if Gimbal_TOP_DEFINE  //TBSYCY
		RC_Dbus_Init();
	#endif
	printf("init OK begin \r\n ");
	/*������ʼ����*/
	mTaskCreate(START,Start);
	/*�������������*/
    vTaskStartScheduler();
}
















