#ifndef __CONTROL__H
#define __CONTROL__H
#include "motorMsg.h"
#include "pid.h"

/************************‘› ±∏È÷√*****************/
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
         sCtrlInfo info;
         cPID pid_pos;
         cPID pid_speed;
		 cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID speed,sGeneralPID pos);
         cMotorCtrl(eGMWhichMotor app, eCtrlType ctrlWay,sGeneralPID speed);
         float getCtrlCur(void);
         sCtrlInfo* getCtrlInfo(void);  
		 void setTarget(float target);
         void changeCtrlPIDParam(sGeneralPID& speed,sGeneralPID& pos);
 };
/************************‘› ±∏È÷√*****************/


 void SendMotorCtrlCurrent(cMotorCtrl cur[]); 






#endif /*__CONTROL__HÕ∑Œƒº˛‘§±‡“Î*/
