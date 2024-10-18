#include "debugTask.h"
#include "sys.h"
#include "ChassisTask.h"
#include "encoder.h"
#include "stateAction.h"

/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOSÊ¹ÓÃ
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
#include "judge.h"
#include "ch2gm.h"
/**FreeRTOS*END***************/


#define DEBUG_TASK_PERIODS  10
void Debug_Task(void *pvParameters){
    TickType_t PreviousWakeTime=xTaskGetTickCount();
    while(1){
//		printf("A:%d,%d,%d,%d,%d,%d\n",
//		getDbusMsgFromCanCom().ch0,
//		getDbusMsgFromCanCom().ch1,
//		getDbusMsgFromCanCom().ch2,
//		getDbusMsgFromCanCom().ch3,
//		getDbusMsgFromCanCom().s1,
//		getDbusMsgFromCanCom().s2);
		//printf("A:%d\n",Judge_Game_State().game_progress);
//		 printf("Hp:%d,%d,%d,%d,%.3f,%d,%d,%d,%.2f,%d,%d\n",
//		 		Judge_Robot_HP().blue_7_robot_HP,
//		 		Judge_Robot_HP().red_outpost_HP,
//		 		Judge_GetRobot_power_heat()->chassis_volt,
//		 		Judge_GetRobot_power_heat()->chassis_current,
//		 		Judge_GetRobot_power_heat()->chassis_power,
//				Judge_GetRobot_power_heat()->chassis_power_buffer,
//				Judge_GetRobot_power_heat()->shooter_id1_17mm_cooling_heat,
//				JudgeRemainBulletNum()->bullet_remaining_num_17mm,
//				Judge_shoot_data()->bullet_speed,
//				Judge_shoot_data()->bullet_freq	,
//				(Judge_Robot_State()).robot_id
//				
//		 );
		//MotorDebugPrint(CMBrake_e);
		//encoderPrint(CMBrake_e); 
        //printf("sensor:%d,%d\n",SensorLeft,SensorRight);
//		printf("A:%d,%d,%.2f,%d\n",(*Judge_GetRobot_power_heat()).shooter_id1_17mm_cooling_heat,
//			JudgeRemainBulletNum()->bullet_remaining_num_17mm,
//			Judge_shoot_data()->bullet_speed,
//			Judge_shoot_data()->bullet_freq	
//		);
		vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(DEBUG_TASK_PERIODS));
    }
}