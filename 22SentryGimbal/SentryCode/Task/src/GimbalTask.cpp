#include "GimabalTask.h"
#include "control.h"
#include <memory>
#include "encoder.h"
#include "imu.h"
#include "VisionCom.h"
#include "dbus.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/

/************yaw轴电机速度环***********/
#define AUTO_AIM_YAW_VC_KP			200.0f	//660.0f
#define AUTO_AIM_YAW_VC_KI			10.0f	//26.5f
#define AUTO_AIM_YAW_VC_KD			0.0f
#define AUTO_AIM_YAW_VC_ERRALLMAX 	1500
#define AUTO_AIM_YAW_VC_OUTMAX		30000		//M6020最大30000 
#define AUTO_AIM_YAW_VC_OUTSTEP		5000
#define AUTO_AIM_YAW_VC_PRECISION 	0.0f
#define AUTO_AIM_YAW_VC_STATE        Normal_e

/************yaw轴电机位置环************/
#define AUTO_AIM_YAW_PC_KP			1.02900f
#define AUTO_AIM_YAW_PC_KI			0.00100f
#define AUTO_AIM_YAW_PC_KD			0.0f
#define AUTO_AIM_YAW_PC_ERRALLMAX 	500
#define AUTO_AIM_YAW_PC_OUTMAX		50
#define AUTO_AIM_YAW_PC_OUTSTEP		5
#define AUTO_AIM_YAW_PC_PRECISION 	0.0f
#define AUTO_AIM_YAW_PC_STATE        Normal_e

/************pitch轴电机速度环***********/			
#define AUTO_AIM_PITCH_VC_KP			200.0f		//660.0f
#define AUTO_AIM_PITCH_VC_KI			2.6f       //26.5f
#define AUTO_AIM_PITCH_VC_KD			0.0f
#define AUTO_AIM_PITCH_VC_ERRALLMAX 	2000
#define AUTO_AIM_PITCH_VC_OUTMAX		30000		//M6020最大30000 
#define AUTO_AIM_PITCH_VC_OUTSTEP		3000
#define AUTO_AIM_PITCH_VC_PRECISION 	0.0f
#define AUTO_AIM_PITCH_VC_STATE        Normal_e

/************pitch轴电机位置环************/
#define AUTO_AIM_PITCH_PC_KP			0.850014f
#define AUTO_AIM_PITCH_PC_KI			00.00000f
#define AUTO_AIM_PITCH_PC_KD			0.0f
#define AUTO_AIM_PITCH_PC_ERRALLMAX 	500
#define AUTO_AIM_PITCH_PC_OUTMAX		100
#define AUTO_AIM_PITCH_PC_OUTSTEP		10
#define AUTO_AIM_PITCH_PC_PRECISION 	0.0f
#define AUTO_AIM_PITCH_PC_STATE        Normal_e
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
//!!!!!!!!!!!!!!!!!!!!!!!  end  !!!!!!!!!!!!!!!!!!!!!!//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//



/************Rammer电机速度环***********/
#define RAMMER_VC_KP			15.00f	//630.0f	//660.0f
#define RAMMER_VC_KI			0.5f	//182.1f	//26.5f
#define RAMMER_VC_KD			0.00f
#define RAMMER_VC_ERRALLMAX 	1000
#define RAMMER_VC_OUTMAX		9000		
#define RAMMER_VC_OUTSTEP		2000
#define RAMMER_VC_PRECISION 	0.0f
const ePID_RampState  RAMMER_VC_STATE  =      Ramp_e;
#define RAMMER_VC_RAMPSTEP      200

/************FRIc电机速度环***********/
#define FRICITION_0_VC_KP			10.30f	//630.0f	//660.0f
#define FRICITION_0_VC_KI			0.30f	//182.1f	//26.5f
#define FRICITION_0_VC_KD			0.00f
#define FRICITION_0_VC_ERRALLMAX 	200
#define FRICITION_0_VC_OUTMAX		16000		
#define FRICITION_0_VC_OUTSTEP		2000
#define FRICITION_0_VC_PRECISION 	0.0f
const ePID_RampState  FRICITION_0_VC_STATE  =      Ramp_e;
#define FRICITION_0_VC_RAMPSTEP      200

/************fric1电机速度环***********/
#define FRICITION_1_VC_KP			10.30f	//630.0f	//660.0f
#define FRICITION_1_VC_KI			0.30f	//182.1f	//26.5f
#define FRICITION_1_VC_KD			0.00f
#define FRICITION_1_VC_ERRALLMAX 	200
#define FRICITION_1_VC_OUTMAX		16000		
#define FRICITION_1_VC_OUTSTEP		2000
#define FRICITION_1_VC_PRECISION 	0.0f
const ePID_RampState FRICITION_1_VC_STATE   =     Ramp_e;
#define FRICITION_1_VC_RAMPSTEP      200


//!!!!!!!!!!!!!!!!!!!!!    auto aim  begin   !!!!!!!!!!!!!!!!!!!!!!//
//yaw双环结构体
sGeneralPID AUTO_AIM_YAW_VC_PID_Struct(AUTO_AIM_YAW_VC_KP,AUTO_AIM_YAW_VC_KI,AUTO_AIM_YAW_VC_KD,AUTO_AIM_YAW_VC_ERRALLMAX,AUTO_AIM_YAW_VC_OUTMAX,AUTO_AIM_YAW_VC_OUTSTEP,AUTO_AIM_YAW_VC_PRECISION,AUTO_AIM_YAW_VC_STATE);
sGeneralPID AUTO_AIM_YAW_PC_PID_Struct(AUTO_AIM_YAW_PC_KP,AUTO_AIM_YAW_PC_KI,AUTO_AIM_YAW_PC_KD,AUTO_AIM_YAW_PC_ERRALLMAX,AUTO_AIM_YAW_PC_OUTMAX,AUTO_AIM_YAW_PC_OUTSTEP,AUTO_AIM_YAW_PC_PRECISION,AUTO_AIM_YAW_PC_STATE);
//pitch双环结构体
sGeneralPID AUTO_AIM_PITCH_VC_PID_Struct(AUTO_AIM_PITCH_VC_KP,AUTO_AIM_PITCH_VC_KI,AUTO_AIM_PITCH_VC_KD,AUTO_AIM_PITCH_VC_ERRALLMAX,AUTO_AIM_PITCH_VC_OUTMAX,AUTO_AIM_PITCH_VC_OUTSTEP,AUTO_AIM_PITCH_VC_PRECISION,AUTO_AIM_PITCH_VC_STATE);
sGeneralPID AUTO_AIM_PITCH_PC_PID_Struct(AUTO_AIM_PITCH_PC_KP,AUTO_AIM_PITCH_PC_KI,AUTO_AIM_PITCH_PC_KD,AUTO_AIM_PITCH_PC_ERRALLMAX,AUTO_AIM_PITCH_PC_OUTMAX,AUTO_AIM_PITCH_PC_OUTSTEP,AUTO_AIM_PITCH_PC_PRECISION,AUTO_AIM_PITCH_PC_STATE);
//!!!!!!!!!!!!!!!!!!!!!    auto aim  end   !!!!!!!!!!!!!!!!!!!!!!//
//拨弹路速度环
sGeneralPID RAMMER_VC_PID_Struct(RAMMER_VC_KP,RAMMER_VC_KI,RAMMER_VC_KD,RAMMER_VC_ERRALLMAX,RAMMER_VC_OUTMAX,RAMMER_VC_OUTSTEP,RAMMER_VC_PRECISION,RAMMER_VC_STATE,RAMMER_VC_RAMPSTEP);
//摩擦轮0速度环
sGeneralPID FRICITION_0_VC_PID_Struct(FRICITION_0_VC_KP,FRICITION_0_VC_KI,FRICITION_0_VC_KD,FRICITION_0_VC_ERRALLMAX,FRICITION_0_VC_OUTMAX,FRICITION_0_VC_OUTSTEP,FRICITION_0_VC_PRECISION,FRICITION_0_VC_STATE,FRICITION_0_VC_RAMPSTEP);
//摩擦轮1速度环
sGeneralPID FRICITION_1_VC_PID_Struct(FRICITION_1_VC_KP,FRICITION_1_VC_KI,FRICITION_1_VC_KD,FRICITION_1_VC_ERRALLMAX,FRICITION_1_VC_OUTMAX,FRICITION_1_VC_OUTSTEP,FRICITION_1_VC_PRECISION,FRICITION_1_VC_STATE,FRICITION_1_VC_RAMPSTEP);

//控制类 对象数组
cMotorCtrl GMAllMotorCtrl[5]={
    {GMYAW_e,posCtrl_e,AUTO_AIM_YAW_VC_PID_Struct,AUTO_AIM_YAW_PC_PID_Struct},
    {GMPitch_e,posCtrl_e,AUTO_AIM_PITCH_VC_PID_Struct,AUTO_AIM_PITCH_PC_PID_Struct},
    {GMRammer_e,speedCtrl_e,RAMMER_VC_PID_Struct},
    {GMFriction0_e,speedCtrl_e,FRICITION_0_VC_PID_Struct},
    {GMFriction1_e,speedCtrl_e,FRICITION_1_VC_PID_Struct}
};
#define GIMBAL_TASK_PERIODS  2

#define FRIC_SPEED_LOW	2000
#define FRIC_SPEED_MID  4000
#define FRIC_SPEED_HIGH 6000
#define LEFT_DIR 	-1
#define RIGHT_DIR	1

#define RAMMER_SPEED_LOW 2000
#define RAMMER_SPEED_MID 2500
#define RAMMER_SPEED_HIGH 3000
#define RAMMER_POS_DIR 1
#define RAMMER_NEG_DIR -1
#define RAMMER_SPEED_INV  500

#define PITCH_MAX 17
#define PITCH_MIN -13

uint16_t reverse_time_ = 0;
bool block_flag = false;


void Gimbal_Task(void *pvParameters){
    TickType_t PreviousWakeTime=xTaskGetTickCount();
	vTaskDelay(2000);
	GMCtrlSetTarget(GMRammer_e,0);
	GMCtrlSetTarget(GMFriction0_e,0);
	GMCtrlSetTarget(GMFriction1_e,0);
	RC_CtrlTypeDef* rc_info = getRCInfo();
	float yaw_target_angel = getMotorMsgFromCan(GMYAW_e)->angelAll;
	float pitch_target_angel =getMotorMsgFromCan(GMPitch_e)->angelAll;
	
	while(1){
		
		//控制代码
		if(rc_info->rc.s2 == 1){ //右上 控制云台
			yaw_target_angel += (float)(rc_info->rc.ch2) * 0.00815;
			pitch_target_angel += rc_info->rc.ch3 * 0.00715;
			if(pitch_target_angel > PITCH_MAX) pitch_target_angel = PITCH_MAX;
			if(pitch_target_angel < PITCH_MIN) pitch_target_angel = PITCH_MIN;
			GMCtrlSetTarget(GMYAW_e,yaw_target_angel);
			GMCtrlSetTarget(GMPitch_e,pitch_target_angel);	
			sendRCInfoToChassis(rc_info,0);
		}
		else if(rc_info->rc.s2 == 2){ // 右下 发送底盘数据
			//sendRCInfoToChassis(rc_info,1);
			GMCtrlSetTarget(GMYAW_e,yaw_target_angel);
			GMCtrlSetTarget(GMPitch_e,pitch_target_angel);
		}
		else if(rc_info->rc.s2 == 3){ // 右中
			yaw_target_angel += (float)(rc_info->rc.ch2) * 0.01215;
			pitch_target_angel += rc_info->rc.ch3 * 0.01215;
			if(pitch_target_angel > PITCH_MAX) pitch_target_angel = PITCH_MAX;
			if(pitch_target_angel < PITCH_MIN) pitch_target_angel = PITCH_MIN;
			GMCtrlSetTarget(GMYAW_e,yaw_target_angel);
			GMCtrlSetTarget(GMPitch_e,pitch_target_angel);	
			sendRCInfoToChassis(rc_info,0);
		}
		
		
		//摩擦轮测速
        if(rc_info->rc.s1 == 2){
            GMCtrlSetTarget(GMFriction0_e,0);
            GMCtrlSetTarget(GMFriction1_e,0);
            GMCtrlSetTarget(GMRammer_e,0);
        }
        else if (rc_info->rc.s1 == 3){
		    GMCtrlSetTarget(GMFriction0_e,LEFT_DIR*FRIC_SPEED_LOW);
            GMCtrlSetTarget(GMFriction1_e,RIGHT_DIR*FRIC_SPEED_LOW);
            if(rc_info->rc.ch1 > 500){
				//lockedRotorPrc(block_flag);
				if(!block_flag)					
					GMCtrlSetTarget(GMRammer_e,RAMMER_POS_DIR * RAMMER_SPEED_LOW);
            }
            else if (rc_info->rc.ch1 < -300){
                GMCtrlSetTarget(GMRammer_e,RAMMER_NEG_DIR*RAMMER_SPEED_INV);
            } 
			else 
				GMCtrlSetTarget(GMRammer_e,0);
        } 
        else if(rc_info->rc.s1 == 1){
            GMCtrlSetTarget(GMFriction0_e,LEFT_DIR*FRIC_SPEED_HIGH);
            GMCtrlSetTarget(GMFriction1_e,RIGHT_DIR*FRIC_SPEED_HIGH);
            if(rc_info->rc.ch1 > 500){ 
				//卡弹处理
				//lockedRotorPrc(block_flag);
				if(!block_flag)					
					GMCtrlSetTarget(GMRammer_e,RAMMER_POS_DIR * RAMMER_SPEED_MID);
			}
			else if (rc_info->rc.ch1 < -300){
                GMCtrlSetTarget(GMRammer_e,RAMMER_NEG_DIR*RAMMER_SPEED_INV);
            } 
			else {
				GMCtrlSetTarget(GMRammer_e,0);
			}
        }			

		//发送电流
        SendMotorCtrlCurrent(GMAllMotorCtrl);
        vTaskDelayUntil(&PreviousWakeTime,pdMS_TO_TICKS(GIMBAL_TASK_PERIODS));
}
	}


void GMCtrlSetTarget(eGMWhichMotor app, float target){
	GMAllMotorCtrl[app].setTarget(target);
}

void MotorSetPID(eGMWhichMotor app,eCtrlType ctrl,float kp, float ki, float kd){
	if(ctrl == posCtrl_e) 
		GMAllMotorCtrl[app].pid_pos.setPID(kp,ki,kd);
	else 
		GMAllMotorCtrl[app].pid_speed.setPID(kp,ki,kd);
}

void sendRCInfoToChassis(RC_CtrlTypeDef* rc_info,int flag){
		CanTxMsg TxMsg = {0};
		TxMsg.StdId = 0x401;
		TxMsg.RTR = CAN_RTR_DATA;
		TxMsg.IDE = CAN_Id_Standard;
		TxMsg.DLC = 6;
		if(flag == 1){
			TxMsg.Data[0] = (int16_t)rc_info->rc.ch2 >> 8;
			TxMsg.Data[1] =  (int16_t)rc_info->rc.ch2 & 0x00ff;
			TxMsg.Data[2] = (int16_t)rc_info->rc.ch3 >> 8;
			TxMsg.Data[3] =  (int16_t)rc_info->rc.ch3 & 0x00ff;
			TxMsg.Data[4] = (int16_t)rc_info->rc.ch0 >> 8;
			TxMsg.Data[5] =  (int16_t)rc_info->rc.ch0 & 0x00ff;		
		}
		CAN_Transmit(CAN2, &TxMsg);
}



float limitMinMax(float data,float min, float max){
	if(data < min) data = min;
	if(data > max) data = max;
	return data;
};


void lockedRotorPrc(bool& block_flag){
//	printf("A:%d,%d,%d,%d\n",
//			getMotorMsgFromCan(GMRammer_e)->torque 
//			,abs( getMotorMsgFromCan(GMRammer_e)->speed) <100 
//			,reverse_time_ == 0 
//			,block_flag == false);
	
	if(getMotorMsgFromCan(GMRammer_e)->torque > 5000 * RAMMER_POS_DIR
			&& abs( getMotorMsgFromCan(GMRammer_e)->speed) < 10 
			&& reverse_time_ == 0 && block_flag == false)
	{
			block_flag = true;
			reverse_time_ = 500;
	}
	if(reverse_time_){
		reverse_time_ -- ;
		GMCtrlSetTarget(GMRammer_e,1500*RAMMER_NEG_DIR);
	}
	else{
		block_flag = false;
	}
}



void MotorDebugPrint(eGMWhichMotor app){
    if(app == GMYAW_e){
        printf("yaw:%.2f,%.2f,%.2f,%.2f,%.2f,%d\n", 
                        GMAllMotorCtrl[app].pid_pos.getPIDStruct().Target,
                        GMAllMotorCtrl[app].pid_pos.getPIDStruct().Input,
			
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Target,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Input,
			
												GMAllMotorCtrl[app].pid_speed.getPIDStruct().Out,
                        getMotorMsgFromCan(app)->torque
        );
    }
    else if(app == GMPitch_e){
        printf("PITCH:%.2f,%.2f,%.2f,%.2f,%.2f,%d\n", 	
			
                        GMAllMotorCtrl[app].pid_pos.getPIDStruct().Target,
                        GMAllMotorCtrl[app].pid_pos.getPIDStruct().Input,
			
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Target,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Input,
			
												GMAllMotorCtrl[app].pid_speed.getPIDStruct().Out,
                        getMotorMsgFromCan(app)->torque
		
        );

    }
    else if(app == GMRammer_e){
        printf("RAMMER:%.2f,%.2f,%.2f,%.2f,%d\n",
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().RampTarget,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Target,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Input,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Out,
                        getMotorMsgFromCan(app)->torque
        );
    }
    else if(app == GMFriction0_e){
        printf("FRIC0:%.2f,%.2f,%.2f,%.2f,%d\n",
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().RampTarget,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Target,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Input,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Out,
                        getMotorMsgFromCan(app)->torque
        );
    }
    else if(app == GMFriction1_e){
        printf("FRIC1:%.2f,%.2f,%.2f,%.2f,%d\n",
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().RampTarget,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Target,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Input,
                        GMAllMotorCtrl[app].pid_speed.getPIDStruct().Out,
                        getMotorMsgFromCan(app)->torque
        );
    }
    else return ;
}

