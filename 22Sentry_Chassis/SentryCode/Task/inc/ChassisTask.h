#ifndef  __MOTOR_TASK_H__
#define	 __MOTOR_TASK_H__

#include "motorMsg.h"
#include "pid.h"
#include "control.h"

void MotorDebugPrint(eGMWhichMotor app);

sGeneralPID& getMotorPIDparam(eGMWhichMotor app,eCtrlType type);







void CMCtrlSetTarget(eGMWhichMotor app, float target);

#endif 