#ifndef __CONTROL__H
#define __CONTROL__H
#include "motorMsg.h"
#include "pid.h"

/************************‘› ±∏È÷√*****************/
#define TOP_GIMBAL_PITCH_MAX	9	
#define TOP_GIMBAL_PITCH_MIN	-14
#define TOP_GIMBAL_YAW_MAX	100
#define TOP_GIMBAL_YAW_MIN	-100

#define BOTTOM_GIMBAL_PITCH_MAX		-110	
#define BOTTOM_GIMBAL_PITCH_MIN		-145

 typedef enum{
     posCtrl_e = 0,
     speedCtrl_e 
 } eCtrlType;

 struct sCtrlInfo{
     eGMWhichMotor apply;
     eCtrlType   ctrlType;  
 };

 class cMotorCtrl{
     private:
         float getSpeedCtrlOut(void);
         float curOutLimit(float curOut);
	 public:
         cPID pid_pos;
         cPID pid_speed;
         sCtrlInfo info;
		 cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID& speed,sGeneralPID& pos);
         cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID& speed);
         float getCtrlCur(void);
         sCtrlInfo* getCtrlInfo(void);  
		 void setTarget(float target);
         void changeCtrlPIDParam(sGeneralPID& speed,sGeneralPID& pos);
         bool isAccOK(void);
		 void setPIDparam(eCtrlType ctrl,float kp,float ki,float kd);
	 
 };
/************************‘› ±∏È÷√*****************/
void SendMotorCtrlCurrent(cMotorCtrl cur[]); 
#endif /*__CONTROL__HÕ∑Œƒº˛‘§±‡“Î*/
