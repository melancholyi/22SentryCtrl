#include "debugTask.h"
#include "imu.h"
#include "encoder.h"
#include "GimabalTask.h"
#include "dbus.h"
#include "gm2ch.h"

/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/
#define DEBUG_TASK_PERIODS  10


void Debug_Task(void *pvParameters){
	TickType_t PreviousWakeTime=xTaskGetTickCount();
	sJudegeMsg judge_msg;
	RC_CtrlTypeDef* rc_info = getRCInfo();
    while(1){
//		printf("A:%d,%d,%d,%d,%d,%d\n",
//				rc_info->rc.s1,
//				rc_info->rc.s2,
//				rc_info->rc.ch0,
//				rc_info->rc.ch1,
//				rc_info->rc.ch2,
//				rc_info->rc.ch3
//		);
		

		MotorDebugPrint(GMPitch_e);
		//encoderPrint(GMPitch_e);
		//printf("msg:%d,%.2f,%d,%.2f\n",getMotorMsgFromCan(GMYAW_e)->speed,get_imu_wz_yaw_rpm(),getMotorMsgFromCan(GMYAW_e)->speed,get_imu_wz_yaw_rpm());
        //绝对延时
		vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(DEBUG_TASK_PERIODS));
    }
}

