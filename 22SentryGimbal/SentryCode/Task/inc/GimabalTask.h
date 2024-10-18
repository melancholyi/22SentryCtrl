#ifndef __GIMBALTASK__H
#define __GIMBALTASK__H

#include "motorMsg.h"
#include "control.h"
#include "dbus.h"

void GMCtrlSetTarget(eGMWhichMotor app, float target);
void A2SchangePIDParam(void);
void MotorSetPID(eGMWhichMotor app,eCtrlType ctrl,float kp, float ki, float kd);

void S2AchangePIDParam(void);
void MotorDebugPrint(eGMWhichMotor app);
bool isFric01ACCOK(void);
void lockedRotorPrc(bool& block_flag);
float limitMinMax(float data,float min, float max);
void sendRCInfoToChassis(RC_CtrlTypeDef* rc_info,int flag);


#endif /*__GIMBALTASK__H*/