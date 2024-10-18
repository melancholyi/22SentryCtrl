/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/
#include "stateAction.h"
#include "VisionCom.h"
#include "gm2ch.h"
#include "dbus.h"
#include "GimabalTask.h"
#include "encoder.h"
#include <cmath>
#include "control.h"
#include "GimabalTask.h"

//cStateAction stateCtrl;
#define STATE_TASK_PERIODS  1
void State_Task(void *pvParameters){
    TickType_t PreviousWakeTime=xTaskGetTickCount();

    //局部变量
	bool block_flag = false;
	
    while(1){
//		//scout
//		if(getUpperCtrlInfo()->sentryMode == sentry_scout_e ){
//			stateCtrl.setState(scout_e);
//		}
//		//auto aim
//		else if(getUpperCtrlInfo()->sentryMode == sentry_autoAim_e ){
//			//auto aim
//			stateCtrl.setState(autoAim_e); //设置模式
//			stateCtrl.setFireLv(fireSpeed_high_e); //设置速度
//			
//			//比赛开始begin 
//			//printf("S:%d\n",getJudgeMsgFromCan()->gameProIsBegin);
//			if(getJudgeMsgFromCan()->gameState == 5){
//					stateCtrl.setFireLv(fireSpeed_stop_e);
//					stateCtrl.setFireFreq(fireFreq_stop_e);
//			}
//			if(getJudgeMsgFromCan()->gameState == 4){
//				
//				if(stateCtrl.isCouldShoot()){
//					//卡弹处理
//					stateCtrl.lockedRotorPrc(block_flag);
//					//热量控制 begin	
//					if(!block_flag)
//						//测试版本 
//						//GMCtrlSetTarget(GMRammer_e,5000 * -1);
//						stateCtrl.shootHeatCtrl(getJudgeMsgFromCan()->shoot_heat);
//					//热量控制 end
//				}

//			}//比赛未开始end
//			else {//比赛未开始 遥控器控制begin
//		if(stateCtrl.isCouldShoot()){
//				#if Gimbal_TOP_DEFINE  //TBSYCY1
//				if(RC_GetDatas().rc.ch3 > 1200){
//					//卡弹处理
//					stateCtrl.lockedRotorPrc(block_flag);
//					//end
//					//热量控制 begin
//					if(!block_flag)
//						stateCtrl.shootHeatCtrl(getJudgeMsgFromCan()->shootHeat_Lv);
//				}
//				else if(RC_GetDatas().rc.ch3 <500){
//					GMCtrlSetTarget(GMRammer_e,-1000);
//				}	
//				else 
//					stateCtrl.setFireFreq(fireFreq_stop_e);
//			
//			#elif  Gimbal_BOTTOM_DEFINE 		
//				if(getDbusMsgFromCanCom().ch3 > 1300){
//					//卡弹处理
//					stateCtrl.lockedRotorPrc(block_flag);
//					//end
//					//热量控制 begin
//					if(!block_flag)
//						stateCtrl.shootHeatCtrl(getJudgeMsgFromCan()->shootHeat_Lv);
//				}
//				else if(RC_GetDatas().rc.ch3 <500){
//					GMCtrlSetTarget(GMRammer_e,-1000);
//				}	
//				else 
//					stateCtrl.setFireFreq(fireFreq_stop_e);
//				
//			#endif 	
//				}

//			}//比赛未开始 遥控器控制end
//			
//		}//视觉控制可以发弹end
//		
//		else{ //prepare state		
//			stateCtrl.setFireFreq(fireFreq_stop_e);
//			stateCtrl.setState(dbus_e);		
//		}
        //stateCtrl.stateActionCtrlLoop();
        vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(STATE_TASK_PERIODS));
    
	}//while
	
}






