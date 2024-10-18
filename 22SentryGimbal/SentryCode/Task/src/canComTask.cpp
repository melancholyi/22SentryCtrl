#include "canComTask.h"
#include "gm2ch.h"
#include "VisionCom.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS π”√
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/

#define CANCOM_TASK_PERIODS  10


void Cancom_Task(void *pvParameters){
	TickType_t PreviousWakeTime=xTaskGetTickCount();
	MSGsendToUpper_TypeDef bulletV_pack ;
	MSGsendToUpper_TypeDef color_pack;
	sJudegeMsg judge_msg;
    while(1){
		TopGimSendDbusMsg2OthersViaCan2();		
		vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(CANCOM_TASK_PERIODS));
    }
}