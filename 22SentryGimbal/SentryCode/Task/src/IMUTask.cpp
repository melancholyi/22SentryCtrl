#include "imu.h"
#include "encoder.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/
#define IMU_TASK_PERIODS  5


void Imu_Task(void *pvParameters){
	TickType_t PreviousWakeTime=xTaskGetTickCount();
    while(1){
		imu_main();
		//printf("A:%.2f,%d,%.2f,%d\n",get_imu_wz_yaw_rpm(),getMotorMsgFromCan(GMYAW_e)->speed,get_imu_wy_pitch_rpm(),getMotorMsgFromCan(GMPitch_e)->speed);
        //绝对延时
		vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(IMU_TASK_PERIODS));
    }
}

