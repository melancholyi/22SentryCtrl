#ifndef __ENCODER__H_
#define __ENCODER__H_
#include "motorMsg.h"

sMotorMsg* getMotorMsgFromCan(eGMWhichMotor whichMotor);
sApplyInfo* getMotorApplyInfo(eGMWhichMotor which);






//½Ó¿Ú²âÊÔº¯Êý
void encoderPrint(eGMWhichMotor apps);
#endif