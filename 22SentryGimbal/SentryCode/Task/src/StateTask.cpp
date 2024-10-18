/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOSʹ��
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

    //�ֲ�����
	bool block_flag = false;
	
    while(1){
//		//scout
//		if(getUpperCtrlInfo()->sentryMode == sentry_scout_e ){
//			stateCtrl.setState(scout_e);
//		}
//		//auto aim
//		else if(getUpperCtrlInfo()->sentryMode == sentry_autoAim_e ){
//			//auto aim
//			stateCtrl.setState(autoAim_e); //����ģʽ
//			stateCtrl.setFireLv(fireSpeed_high_e); //�����ٶ�
//			
//			//������ʼbegin 
//			//printf("S:%d\n",getJudgeMsgFromCan()->gameProIsBegin);
//			if(getJudgeMsgFromCan()->gameState == 5){
//					stateCtrl.setFireLv(fireSpeed_stop_e);
//					stateCtrl.setFireFreq(fireFreq_stop_e);
//			}
//			if(getJudgeMsgFromCan()->gameState == 4){
//				
//				if(stateCtrl.isCouldShoot()){
//					//��������
//					stateCtrl.lockedRotorPrc(block_flag);
//					//�������� begin	
//					if(!block_flag)
//						//���԰汾 
//						//GMCtrlSetTarget(GMRammer_e,5000 * -1);
//						stateCtrl.shootHeatCtrl(getJudgeMsgFromCan()->shoot_heat);
//					//�������� end
//				}

//			}//����δ��ʼend
//			else {//����δ��ʼ ң��������begin
//		if(stateCtrl.isCouldShoot()){
//				#if Gimbal_TOP_DEFINE  //TBSYCY1
//				if(RC_GetDatas().rc.ch3 > 1200){
//					//��������
//					stateCtrl.lockedRotorPrc(block_flag);
//					//end
//					//�������� begin
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
//					//��������
//					stateCtrl.lockedRotorPrc(block_flag);
//					//end
//					//�������� begin
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

//			}//����δ��ʼ ң��������end
//			
//		}//�Ӿ����ƿ��Է���end
//		
//		else{ //prepare state		
//			stateCtrl.setFireFreq(fireFreq_stop_e);
//			stateCtrl.setState(dbus_e);		
//		}
        //stateCtrl.stateActionCtrlLoop();
        vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(STATE_TASK_PERIODS));
    
	}//while
	
}






