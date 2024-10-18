#include "control.h"
#include <memory>
#include "ChassisTask.h"
#include "encoder.h"
#include "judge.h"
#include "ch2gm.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/

/************driving 电机速度环***********/
#define DRIVING_VC_KP			15.00f	//630.0f	//660.0f
#define DRIVING_VC_KI			1.0f	//182.1f	//26.5f
#define DRIVING_VC_KD			0.00f
#define DRIVING_VC_ERRALLMAX 	500
#define DRIVING_VC_OUTMAX		9000		
#define DRIVING_VC_OUTSTEP		2000
#define DRIVING_VC_PRECISION 	0.0f
#define DRIVING_VC_STATE        Ramp_e
#define DRIVING_VC_RAMPSTEP     250



/************brake电机速度环***********/
#define BRAKE_VC_KP			9.2f		
#define BRAKE_VC_KI			0.63f
#define BRAKE_VC_KD			0.0f
#define BRAKE_VC_ERRALLMAX 	2000
#define BRAKE_VC_OUTMAX		9000		
#define BRAKE_VC_OUTSTEP	2000
#define BRAKE_VC_PRECISION 	0.000f
#define BRAKE_VC_STATE        Normal_e

/************brake电机位置环************/
#define BRAKE_PC_KP			250.52f
#define BRAKE_PC_KI			0.000f
#define BRAKE_PC_KD			0.0f
#define BRAKE_PC_ERRALLMAX 	500
#define BRAKE_PC_OUTMAX		5000
#define BRAKE_PC_OUTSTEP	2500
#define BRAKE_PC_PRECISION 	0.0001f
#define BRAKE_PC_STATE      Normal_e

//brake 双环结构体
sGeneralPID BRAKE_VC_PID_Struct(BRAKE_VC_KP,BRAKE_VC_KI,BRAKE_VC_KD,BRAKE_VC_ERRALLMAX,BRAKE_VC_OUTMAX,BRAKE_VC_OUTSTEP,BRAKE_VC_PRECISION,BRAKE_VC_STATE);
sGeneralPID BRAKE_PC_PID_Struct(BRAKE_PC_KP,BRAKE_PC_KI,BRAKE_PC_KD,BRAKE_PC_ERRALLMAX,BRAKE_PC_OUTMAX,BRAKE_PC_OUTSTEP,BRAKE_PC_PRECISION,BRAKE_PC_STATE);

//driving速度环
sGeneralPID DRIVING_VC_PID_Struct(DRIVING_VC_KP,DRIVING_VC_KI,DRIVING_VC_KD,DRIVING_VC_ERRALLMAX,DRIVING_VC_OUTMAX,DRIVING_VC_OUTSTEP,DRIVING_VC_PRECISION,DRIVING_VC_STATE,DRIVING_VC_RAMPSTEP);

//控制类 对象数组
cMotorCtrl CMAllMotorCtrl[2]={
    {CMDriving_e,speedCtrl_e,DRIVING_VC_PID_Struct},
    {CMBrake_e,posCtrl_e,BRAKE_VC_PID_Struct,BRAKE_PC_PID_Struct},
};

#define CHASSIS_TASK_PERIODS  2
uint16_t times = 0;
#define TEMP_TEST_SPEED 3500
#define TIME_LOCK 2000
void Chassis_Task(void *pvParameters){
    TickType_t PreviousWakeTime=xTaskGetTickCount();
    sDbusMsgFromTop dbus_msg ;
	
	int16_t time_lock = TIME_LOCK;
	uint16_t times= 0;
 	while(1){	
		SendMotorCtrlCurrent(CMAllMotorCtrl);
        vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(CHASSIS_TASK_PERIODS));
    }
}

void CMCtrlSetTarget(eGMWhichMotor app, float target){
	CMAllMotorCtrl[app].setTarget(target);
}

sGeneralPID& getMotorPIDparam(eGMWhichMotor app,eCtrlType type){
	if(app == CMDriving_e){
		return CMAllMotorCtrl[app].pid_speed.getPIDStruct();
	}
	else {
        if(type == posCtrl_e)
		    return CMAllMotorCtrl[app].pid_pos.getPIDStruct();
        else return CMAllMotorCtrl[app].pid_speed.getPIDStruct();
	}
}
ext_power_heat_data_t* pwr_msg ;

void MotorDebugPrint(eGMWhichMotor app){
	
    if(app == CMDriving_e){
		pwr_msg = Judge_GetRobot_power_heat();
        printf("Driving:%.2f,%.2f,%.2f,%.2f,%d,%.2f,%d\n",
                        CMAllMotorCtrl[app].pid_speed.getPIDStruct().RampTarget,
                        CMAllMotorCtrl[app].pid_speed.getPIDStruct().Target,
                        CMAllMotorCtrl[app].pid_speed.getPIDStruct().Input,
                        CMAllMotorCtrl[app].pid_speed.getPIDStruct().Out,
                        getMotorMsgFromCan(app)->torque,
						pwr_msg->chassis_power,
						pwr_msg->chassis_power_buffer
        );
    }
    else if(app == CMBrake_e){
        printf("Brake:%.3f,%.3f,%.2f,%.2f,%.2f,%d,%.2f\n",  
                        CMAllMotorCtrl[app].pid_pos.getPIDStruct().Target,
                        CMAllMotorCtrl[app].pid_pos.getPIDStruct().Input,
                        CMAllMotorCtrl[app].pid_speed.getPIDStruct().Target,
                        CMAllMotorCtrl[app].pid_speed.getPIDStruct().Input,
                        CMAllMotorCtrl[app].pid_speed.getPIDStruct().Out,
                        getMotorMsgFromCan(app)->torque,
						CMAllMotorCtrl[app].pid_speed.getPIDStruct().Err_now
        );

    }
    else return ;
}



