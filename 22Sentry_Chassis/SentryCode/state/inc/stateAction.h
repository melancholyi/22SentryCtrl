#ifndef  __STATE_ACTION_H
#define  __STATE_ACTION_H
#include "rng.h"
#include "judge.h"
enum eIsBrake{
    noBrake_e = 0 ,
    leftBrake_e = 1 ,
    rightBrake_e = 2
};

enum eIsRandMove{
    stopReady_e  = 0,
    randMove_e = 1,
	dbus_e = 2
};

struct sRandomRange{
    int16_t   min;
    int16_t   max;
    sRandomRange(void){
        memset(&min,0,sizeof(sRandomRange));
    }
};
#define SensorRight		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) //PE5
#define SensorLeft		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) //PE4
#define DetectedColumn  0
#define NO_DetectedColumn  1

class cStateAction{

private:
    //brake value
    float LEFT_BRAKE_POS = 8.5;
    float RIGHT_BRAKE_POS = -18.0;
    float NO_BRAKE_POS = 8.0f;

    bool brake_init_f = false;
    float BRAKE_DELTA_ = 0;
	float com_revDirP_ ; 

private:
    eIsBrake brake_;
    eIsRandMove move_;
    sRandomRange randomV_range_;
    sRandomRange timeLock_range_;
    int16_t time_lock_;
    float rand_move_v_;
    float brake_pos_;
    float last_rand_move_v_;
    bool brake_flag_ = false;
    void randMoveCtrl(void);
    void stopReadyCtrl(void);
	void  dbusCtrl(void);
    int16_t getRandomSpeedAndDir(float p);
    void brakeInit(void);
public:
    cStateAction();
    void setMoveState(const eIsRandMove move);
    void setBrakeState(const eIsBrake brake);
    void setRandomVRange(const int16_t min,const int16_t max);
	void setTimeLockRange(const int16_t min,const int16_t max);
    void ChassisAllStateCtrl(void);
	void setRandMoveWay(uint16_t pwr_buf);
	void setComDirRevP(float p);


};



 




#endif // ! __STATE_ACTION_H
