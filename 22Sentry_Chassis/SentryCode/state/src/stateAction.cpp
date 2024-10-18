#include "stateAction.h"
#include "rng.h"
#include "motorMsg.h"
#include "ChassisTask.h"
#include "judge.h"
#include "ch2gm.h"
#include "encoder.h"
#include "math.h"
#include "control.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/
//#include "MotorTask.h"
const int16_t MOVE_LEFT  = 1;
const int16_t MOVE_RIGHT = -1;

#define  DETECT_COLUMN_TIMELOCK_MULT 1
#define  DETECT_COLUMN_SPEED_MULT	0.95

#define  BRAKE_FLAG_TORQUE			500	

cStateAction::cStateAction() {
    randomV_range_.min = randomV_range_.max = 0;
    move_ = stopReady_e;
    brake_ = noBrake_e;
    time_lock_ = 0;
	timeLock_range_.max = 3000 ;
	timeLock_range_.min = 1000;
    rand_move_v_  = last_rand_move_v_ = 0;
    brake_pos_ = NO_BRAKE_POS;
	com_revDirP_ = 0.5;
}

void cStateAction::setBrakeState(const eIsBrake brake){
    brake_ = brake;
}

void cStateAction::setMoveState(const eIsRandMove move){
    move_ = move;
}

void cStateAction::setRandomVRange(const int16_t min,const int16_t max){
    randomV_range_.min = min;
    randomV_range_.max = max;
}
void cStateAction::setTimeLockRange(const int16_t min, const int16_t max){
	timeLock_range_.min = min;
	timeLock_range_.max = max;
}
int16_t cStateAction::getRandomSpeedAndDir(float p){
    //int16_t dir = getPosOrNegNum();
	//计算反向概率
	int8_t now_dir = (getMotorMsgFromCan(CMDriving_e)->speed > 0) ? 1 : -1;
	int16_t num = getRandomNum(1,10000);
	int dir = (num < 10000*p)? -now_dir : now_dir;
	
    int16_t speed = getRandomNum(randomV_range_.min,randomV_range_.max);
    return (dir * speed);
}

void cStateAction::stopReadyCtrl(void){
    if(SensorLeft != DetectedColumn || SensorRight != DetectedColumn)
        CMCtrlSetTarget(CMDriving_e,5000*MOVE_RIGHT);
    else CMCtrlSetTarget(CMDriving_e,0);
}

template <typename type>
bool isDiff(type tar ,type now){
	if((tar > 0 && now < 0) || (tar < 0 && now > 0)){
		return true;
	}
	else return false;
}
float my_abs(float in){
	return (in>0)?in:-in;
}
bool get_new_v_flag = true;
sMotorMsg drive_temp ;
sMotorMsg brake_temp ;
void cStateAction::randMoveCtrl(void){
/***********************begin 2022/4/1 without brake begin  ************************/
    sMotorMsg drive_temp =  *getMotorMsgFromCan(CMDriving_e);
    //sMotorMsg brake_temp =  *getMotorMsgFromCan(CMBrake_e);
    if(move_ == randMove_e){
        //left detected column
		if(SensorLeft == DetectedColumn){
             time_lock_ = DETECT_COLUMN_TIMELOCK_MULT*getRandomNum(timeLock_range_.min,timeLock_range_.max);
             rand_move_v_ = ((MOVE_RIGHT)*DETECT_COLUMN_SPEED_MULT)*getRandomNum(randomV_range_.min,randomV_range_.max);
        }
        //right detected column
        else if(SensorRight == DetectedColumn){
                time_lock_ = DETECT_COLUMN_TIMELOCK_MULT*getRandomNum(timeLock_range_.min,timeLock_range_.max);
                rand_move_v_ = MOVE_LEFT*DETECT_COLUMN_SPEED_MULT*getRandomNum(randomV_range_.min,randomV_range_.max);
        }
        //normal rand move state
        else {
            if(time_lock_ >0) time_lock_ --;
			else{
				rand_move_v_ = getRandomSpeedAndDir(com_revDirP_);
                time_lock_ = getRandomNum(timeLock_range_.min,timeLock_range_.max);
			}
		}
		CMCtrlSetTarget(CMDriving_e,rand_move_v_);
        //CMCtrlSetTarget(CMBrake_e, brake_pos_);
    }
    else { //stop
        CMCtrlSetTarget(CMDriving_e,0);
    }
/***********************END 2022/4/1 without brake end  ************************/





}

//////
sMotorMsg brake_msg ,drive_msg;
sGeneralPID brake_pid,drive_pid;
/////////////

/*dbus mode*/
void cStateAction::dbusCtrl(void){
	
	CMCtrlSetTarget(CMDriving_e,(getDbusMsgFromCanCom().ch0-1024)*12);
}



void cStateAction::ChassisAllStateCtrl(void){
    if(move_ == stopReady_e){
        stopReadyCtrl();
    }
    else if(move_ == randMove_e){
        randMoveCtrl();
    }
	else if(move_ == dbus_e){
		dbusCtrl();
	}
}

void cStateAction::brakeInit(void){
	float init_angel = getMotorMsgFromCan(CMBrake_e)->angelAll;
	float one_side = 0,other_side = 0;
	float tar = 0;
	brake_init_f = true;		
		
	CMCtrlSetTarget(CMBrake_e,init_angel + 30);
	//while(!(getMotorMsgFromCan(CMBrake_e)->speed == 0 && my_abs(getMotorMsgFromCan(CMBrake_e)->torque) > 2000));
	vTaskDelay(1000);
	one_side = getMotorMsgFromCan(CMBrake_e)->angelAll;
	CMCtrlSetTarget(CMBrake_e,one_side);


	CMCtrlSetTarget(CMBrake_e,init_angel-30);
	//while(!(getMotorMsgFromCan(CMBrake_e)->speed == 0 && my_abs(getMotorMsgFromCan(CMBrake_e)->torque) > 2000));
	vTaskDelay(1000);
	other_side = getMotorMsgFromCan(CMBrake_e)->angelAll;		
	CMCtrlSetTarget(CMBrake_e,other_side);
	NO_BRAKE_POS =( one_side + other_side )/2;
	BRAKE_DELTA_ = my_abs(one_side - other_side)/2+3.0;
	brake_pos_ = NO_BRAKE_POS;
	LEFT_BRAKE_POS = NO_BRAKE_POS - BRAKE_DELTA_;
	RIGHT_BRAKE_POS = NO_BRAKE_POS + BRAKE_DELTA_;
	//printf("tempMsg:%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",init_angel,one_side,other_side,BRAKE_DELTA_,NO_BRAKE_POS,LEFT_BRAKE_POS,RIGHT_BRAKE_POS);
	CMCtrlSetTarget(CMBrake_e,NO_BRAKE_POS);
}

void cStateAction::setRandMoveWay(uint16_t pwr_buf){
	if(pwr_buf >= 150 && pwr_buf <=200){
		setRandomVRange(8000,9000);
		setTimeLockRange(2500,3000);
	}
	else if(pwr_buf >= 100 && pwr_buf <150){
		setRandomVRange(6500,7500);
		setTimeLockRange(2500,3000);
	}
	else if(pwr_buf >=50 && pwr_buf <100){
		setRandomVRange(6000,7000);
		setTimeLockRange(2000,2500);
	}
	else {
		setRandomVRange(5000,6000);
		setTimeLockRange(2000,2500);
	}	
}

void cStateAction::setComDirRevP(float p){
	com_revDirP_ = p;
}
