#include "pid.h"
#include "encoder.h"
#include "control.h"
#include <vector>

cMotorCtrl::cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID speed,sGeneralPID pos){
 info.apply = app;
 info.ctrlType = ctrlWay;
 pid_pos = pos;
 pid_speed = speed;
}
cMotorCtrl::cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID speed){
 info.apply = app;
 info.ctrlType = ctrlWay;
 pid_speed = speed;
}
float cMotorCtrl::getSpeedCtrlOut(void){
   pid_speed.updateParam(getMotorMsgFromCan(info.apply)->speed) ;
   return pid_speed.calculate();
}

float cMotorCtrl::getCtrlCur(void){
   //ËÙ¶È¿ØÖÆ
   if(info.ctrlType == speedCtrl_e){
       return getSpeedCtrlOut();
   }
   else if(info.ctrlType == posCtrl_e){
       pid_pos.updateParam(getMotorMsgFromCan(info.apply)->angelAll);
       pid_speed.setTargetWithoutRamp(pid_pos.calculate());
       return getSpeedCtrlOut();
   }
   else return 0;
}
sCtrlInfo* cMotorCtrl::getCtrlInfo(void){
   return &info;
}
float cMotorCtrl::curOutLimit(float curOut){
    float retCur = 0;
    float maxCurLimit = getMotorApplyInfo(info.apply)->MAXCur;
   retCur = retCur > maxCurLimit ? maxCurLimit : retCur < -maxCurLimit ? - maxCurLimit : retCur;
   return retCur;
}
void cMotorCtrl::setTarget(float target){
	if(info.ctrlType == posCtrl_e)
		pid_pos.setTargetWithoutRamp(target);
	else if(info.ctrlType == speedCtrl_e)
		pid_speed.setTargetWithRamp(target);
	else return;
}

int16_t drive_cur=0,brake_cur=0;
static CanTxMsg TxMsg_0x200 = {0};//yyyyyy???????????
void SendMotorCtrlCurrent(cMotorCtrl cur[]){
   drive_cur = (int16_t)cur[CMDriving_e].getCtrlCur();
   brake_cur = (int16_t)cur[CMBrake_e].getCtrlCur();
   TxMsg_0x200.StdId = 0x200;
   TxMsg_0x200.RTR = CAN_RTR_DATA;
   TxMsg_0x200.IDE = CAN_Id_Standard;
   TxMsg_0x200.DLC = 4;
   TxMsg_0x200.Data[0] =  drive_cur>> 8;			
   TxMsg_0x200.Data[1] =  drive_cur& 0x00ff;
   TxMsg_0x200.Data[2] =  brake_cur>> 8;			
   TxMsg_0x200.Data[3] =  brake_cur& 0x00ff;
	//printf("send\n");
   CAN_Transmit(CAN2, &TxMsg_0x200);  
}

void cMotorCtrl::changeCtrlPIDParam(sGeneralPID& speed,sGeneralPID& pos){
    pid_pos = pos;
    pid_speed =speed;
}


































