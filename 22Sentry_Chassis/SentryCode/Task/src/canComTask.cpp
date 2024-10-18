#include "ch2gm.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS π”√
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/



#define CANCOM_TASK_PERIODS	10
void Cancom_Task(void *pvParameters){
	
	TickType_t PreviousWakeTime=xTaskGetTickCount();
    while(1){
		sendJudgeMSGtoGimbal();
        vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(CANCOM_TASK_PERIODS));
    }	

}


















