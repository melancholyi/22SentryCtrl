#include "pid.h"
#include "encoder.h"
#include "control.h"
#include "imu.h"
#include <vector>
#include "gm2ch.h"

#define  PITCH_MULT_PARPAM 1



//下云台pitch的限位 0-35 极限机械 电控限制到2-30就可以 2022/5/21
//向下速度为正


cMotorCtrl::cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID& speed,sGeneralPID& pos){
 info.apply = app;
 info.ctrlType = ctrlWay;
 pid_pos = pos;
 pid_speed = speed;
}

cMotorCtrl::cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID& speed){
 info.apply = app;
 info.ctrlType = ctrlWay;
 pid_speed = speed;
}

float cMotorCtrl::getSpeedCtrlOut(void){
   pid_speed.updateParam(getMotorMsgFromCan(info.apply)->speed) ;
   return pid_speed.calculate();
}

#define ERROR_LV1  2.0
#define ERROR_LV2  1.5
#define ERROR_LV3  1.0
#define ERROR_LV4  0.5
#define ERROR_LV5  0.1

float cMotorCtrl::getCtrlCur(void){
    
   if(info.ctrlType == speedCtrl_e){
       return curOutLimit(getSpeedCtrlOut());
   }
   else if(info.ctrlType == posCtrl_e){
		   pid_pos.updateParam(getMotorMsgFromCan(info.apply)->angelAll);
       pid_speed.setTargetWithoutRamp(pid_pos.calculate());
       return curOutLimit(getSpeedCtrlOut());
   }
   else return 0;
}

sCtrlInfo* cMotorCtrl::getCtrlInfo(void){
   return &info;
}
float cMotorCtrl::curOutLimit(float curOut){
   static float retCur = 0;
   static float maxCurLimit = getMotorApplyInfo(info.apply)->MAXCur;
   retCur = curOut > maxCurLimit ? maxCurLimit : curOut < -maxCurLimit ? - maxCurLimit : curOut;
   return retCur;
}

void cMotorCtrl::setTarget(float target){
		if(info.ctrlType == posCtrl_e)
			pid_pos.setTargetWithoutRamp(target);
		else if(info.ctrlType == speedCtrl_e)
			pid_speed.setTargetWithRamp(target);
		else return;
}

void cMotorCtrl::changeCtrlPIDParam(sGeneralPID& speed,sGeneralPID& pos){
    pid_pos = pos;
    pid_speed =speed;
}
void cMotorCtrl::setPIDparam(eCtrlType ctrl,float kp,float ki,float kd){
	if(ctrl == posCtrl_e){
		pid_pos.setPID(kp,ki,kd);
	}
	else if(ctrl == speedCtrl_e){
		pid_speed.setPID(kp,ki,kd);
	}
}


//F0 = 5    F1 = 6    Rammer = 7   Pitch =  4
static CanTxMsg TxMsg_0x1FF = {0};
void SendMotorCtrlCurrent(cMotorCtrl cur[]){
   
	 TxMsg_0x1FF.StdId = 0x1FF;
   TxMsg_0x1FF.RTR = CAN_RTR_DATA;
   TxMsg_0x1FF.IDE = CAN_Id_Standard;
   TxMsg_0x1FF.DLC = 6;
   TxMsg_0x1FF.Data[0] = (int16_t)cur[GMPitch_e].getCtrlCur() >> 8;			
   TxMsg_0x1FF.Data[1] = (int16_t)cur[GMPitch_e].getCtrlCur() & 0x00ff;
   TxMsg_0x1FF.Data[2] = (int16_t)cur[GMFriction1_e].getCtrlCur() >> 8;			
   TxMsg_0x1FF.Data[3] = (int16_t)cur[GMFriction1_e].getCtrlCur() & 0x00ff;
   TxMsg_0x1FF.Data[4] = (int16_t)cur[GMFriction0_e].getCtrlCur() >> 8;			
   TxMsg_0x1FF.Data[5] = (int16_t)cur[GMFriction0_e].getCtrlCur() & 0x00ff;
   CAN_Transmit(CAN1, &TxMsg_0x1FF);
   
	 
    TxMsg_0x1FF.DLC = 4;
    TxMsg_0x1FF.Data[0] = (int16_t)cur[GMYAW_e].getCtrlCur() >> 8;
    TxMsg_0x1FF.Data[1] = (int16_t)cur[GMYAW_e].getCtrlCur() & 0x00ff;
	  TxMsg_0x1FF.Data[2] = (int16_t)cur[GMRammer_e].getCtrlCur() >> 8;
    TxMsg_0x1FF.Data[3] = (int16_t)cur[GMRammer_e].getCtrlCur() & 0x00ff;
    CAN_Transmit(CAN2, &TxMsg_0x1FF);
}

float my_abs(float num){
    return (num>0?num:-num);
}

bool cMotorCtrl::isAccOK(void){
    return (my_abs(pid_speed.getPIDStruct().Input-pid_speed.getPIDStruct().Target) < 500 && pid_speed.getPIDStruct().Target > 500);
}

