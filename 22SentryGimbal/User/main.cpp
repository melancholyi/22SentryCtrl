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
#include "FreeRTOS.h"					//FreeRTOS使用	 
#include "task.h"
/**FreeRTOS*END***************/

/*开始任务*///用于创建任务、队列、定时器
/*-摘要-*/	#define START
/*-优先-*/	#define START_TASK_PRIO		1
/*-堆栈-*/	#define START_STK_SIZE 		100  
/*-句柄-*/	TaskHandle_t StartTask_Handler;
/*-声明-*/	extern void Start_Task(void *pvParameters);


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


/*通用任务的宏定义在 my_task.h 下*/
int main(void)
{
	
	/*中断分组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    /*滴答定时器配置*/
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
	/*创建开始任务*/
	mTaskCreate(START,Start);
	/*开启任务调度器*/
    vTaskStartScheduler();
}
















