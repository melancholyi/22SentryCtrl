#ifndef __PID__H
#define __PID__H

#include "sys.h"

typedef enum 
{
	Normal_e = 0,				//PID工作在正常状态
	Ramp_e = 1,					//PID工作在斜坡函数状态
	Ramping_Add = 2,		//斜坡还没完成，并且是加
	Ramping_Reduce = 3,	//斜坡还没完成，并且是减
}ePID_RampState;


struct sGeneralPID
{
	float Kp1;								//P				
	float Ki1;								//I
	float Kd1;								//D
	
	float Err_now;				//当前误差
	float Err_last;				//上一次误差
	float Err_lastlast;		//上上次误差
	float Err_all;				//误差积分
	float Target;					//目标
	float Input;					//输入
	float Out;						//输出
	float lastout;				//上一次输出
  
	uint8_t State_RampOrNormal;//PID状态
	
	float RampTarget;					 //斜坡函数的目标
	float RampCountTime;			 //斜坡计数，与 Target作比较来决定是否增加一个步幅
	float RampTartgetTime;		 //计数多少次才把count加上1个步幅	
	float RampTartgetStep;		 //斜坡函数步幅
	
	float ErrAllMax;			//PID积分限幅
	float OutMax;					//PID输出限幅
	float OutStep;				//PID输出步幅限制
		
	float Precision;			//PID最小精度
	//构造函数
	sGeneralPID(){//初始化为零
		memset(&Kp1,0,sizeof(sGeneralPID));
	}
	sGeneralPID(float kp,float ki,float kd , float errAllMax,float outMax,float outStep,float precision,ePID_RampState state,float rampStep){
		memset(&Kp1,0,sizeof(sGeneralPID));
		Kp1 = kp;//
	 	Ki1 = ki;//
	 	Kd1 = kd;//
	 	
	 	ErrAllMax = errAllMax;
	 	OutMax 	= outMax;
	 	OutStep	= outStep;
		
	 	Precision = precision;//
	 	RampTartgetStep = rampStep;
		State_RampOrNormal = state;
	}
	sGeneralPID(float kp,float ki,float kd , float errAllMax,float outMax,float outStep,float precision,ePID_RampState state){
		memset(&Kp1,0,sizeof(sGeneralPID));
		Kp1 = kp;
	 	Ki1 = ki;
	 	Kd1 = kd;
	 	ErrAllMax = errAllMax;
	 	OutMax 	= outMax;
	 	OutStep	= outStep;
	 	Precision = precision;
	 	State_RampOrNormal = state;
	}
};


class cPID{
	private:
		
	sGeneralPID PID;
	
	public:
		
	//构造函数1,目标有斜坡版本
	cPID(float kp,float ki,float kd , float errAllMax,float outMax,float outStep,float precision,ePID_RampState state,float rampStep);
	//构造函数2，目标无斜坡版本
	cPID(float kp,float ki,float kd , float errAllMax,float outMax,float outStep,float precision,ePID_RampState state);
	//构造函数 pid结构体作为参数
	cPID(sGeneralPID copy);
	cPID(void);
	



	//更新PID的数据，即更新PID的输入值
	void updateParam(float NowInput);
	float calculate(void);
	
	void setTargetWithRamp(float tar);
	void setTargetWithoutRamp(float tar);
	float getTarget(void);
	
	
	
	sGeneralPID& getPIDStruct(void);
	void setPID(float kp,float ki,float kd);
	float getABSError(void);
};








#endif 