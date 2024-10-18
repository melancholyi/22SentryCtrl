/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
#include "ChassisTask.h"
/**FreeRTOS*END***************/

#include "StateTask.h"
#include "stateAction.h"
#include "ch2gm.h"
#include "judge.h"
#define STATE_TASK_PERIODS 1


void State_Task(void *pvParameters){
    TickType_t PreviousWakeTime=xTaskGetTickCount();
	cStateAction stateCtrl;
    uint16_t myoutpost_HP = 2000;
	uint16_t mySentry_HP = 600;
	uint16_t lastMyHp = mySentry_HP;
	sDbusMsgFromTop dbus_msg ;
	ext_power_heat_data_t* power_msg;
	uint16_t pwr_buf =200;
	uint16_t bloodReduce_times = 0;
	float blood_reduce_V = 0;
	
    while(1){
		
//		if((Judge_Robot_State()).robot_id<=RedRadar){
//				myoutpost_HP = (Judge_Robot_HP().red_outpost_HP) ;
//				mySentry_HP = (Judge_Robot_HP().red_7_robot_HP);
//		}
//		else if((Judge_Robot_State()).robot_id>=BlueHero){
//				myoutpost_HP = (Judge_Robot_HP().blue_outpost_HP);
//				mySentry_HP = (Judge_Robot_HP().blue_7_robot_HP);
//		}//获取血量end
//		
//		
//		dbus_msg = getDbusMsgFromCanCom();
//		//printf("dbus:%d,%d\n",dbus_msg.s1,dbus_msg.ch0);
//		power_msg = Judge_GetRobot_power_heat();
//		pwr_buf = power_msg->chassis_power_buffer;
//		if(Judge_Game_State().game_progress == 4 || dbus_msg.s1 == 3){
//			stateCtrl.setMoveState(randMove_e);
//			stateCtrl.setRandMoveWay(pwr_buf);//根据pwr_buf进行移动策略
//			stateCtrl.setComDirRevP(0.7);
//		}//设置开关
//		else{
//			stateCtrl.setMoveState(dbus_e);
//		}
//		
//		if(mySentry_HP<580){
//			stateCtrl.setComDirRevP(0.8);
//		}
		
		
		
		
		/*normal state switch*/
		
		if((Judge_Robot_State()).robot_id<=RedRadar){
				myoutpost_HP = (Judge_Robot_HP().red_outpost_HP) ;
				mySentry_HP = (Judge_Robot_HP().red_7_robot_HP);
		}
		else if((Judge_Robot_State()).robot_id>=BlueHero){
				myoutpost_HP = (Judge_Robot_HP().blue_outpost_HP);
				mySentry_HP = (Judge_Robot_HP().blue_7_robot_HP);
		}//获取血量end
		dbus_msg = getDbusMsgFromCanCom();
		if((Judge_Game_State().game_progress == 4 && Judge_Game_State().stage_remain_time<298) || (dbus_msg.s1 == 3)){//比赛开始

			//slove blood decrease V
			if(bloodReduce_times<=1000){
				bloodReduce_times ++;
			}
			else{
				bloodReduce_times = 0;
				blood_reduce_V = (mySentry_HP - lastMyHp)/1000;
				lastMyHp = mySentry_HP;
			}
			
			if(blood_reduce_V > 50){
				//遇见敌人就加快变相
				stateCtrl.setMoveState(randMove_e);
				stateCtrl.setRandMoveWay(pwr_buf);//根据pwr_buf进行移动策略
				stateCtrl.setComDirRevP(0.65);
			}
			else{
				//初始化直接启动
				stateCtrl.setMoveState(randMove_e);
				stateCtrl.setRandMoveWay(pwr_buf);//根据pwr_buf进行移动策略
				stateCtrl.setComDirRevP(0.5);
			}	
		}
		else{ //比赛未开始 遥控器控制
//			/*dbus change*/
//			dbus_msg = getDbusMsgFromCanCom();
//			power_msg = Judge_GetRobot_power_heat();
//			pwr_buf = power_msg->chassis_power_buffer;
//			if(dbus_msg.s1 == 3){
//				stateCtrl.setMoveState(randMove_e);
//				stateCtrl.setRandMoveWay(pwr_buf);//根据pwr_buf进行移动策略
//			}
//			else{
				stateCtrl.setMoveState(dbus_e);
			//}
		}
		
		
		/*******************************/
        stateCtrl.ChassisAllStateCtrl();
        vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(STATE_TASK_PERIODS));
    }
}























