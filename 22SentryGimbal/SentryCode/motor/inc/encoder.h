#ifndef __ENCODER__H_
#define __ENCODER__H_
#include "motorMsg.h"

sMotorMsg* getMotorMsgFromCan(eGMWhichMotor whichMotor);
sApplyInfo* getMotorApplyInfo(eGMWhichMotor which);






//�ӿڲ��Ժ���
void encoderPrint(eGMWhichMotor apps);
#endif