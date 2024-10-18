#include "pid.h"

//PID构造函数1，有斜坡版本
 cPID::cPID(float kp,float ki,float kd,float errAllMax,float outMax,float outStep,float precision,ePID_RampState state,float rampStep){
	 memset(&PID.Kp1,0,sizeof(sGeneralPID));//全部初始为零
	 PID.Kp1 = kp;
	 PID.Ki1 = ki;
	 PID.Kd1 = kd;
	 PID.RampTartgetStep = rampStep;
	 PID.ErrAllMax = errAllMax;
	 PID.OutMax 	= outMax;
	 PID.OutStep	= outStep;
	 PID.Precision = precision;
	 PID.State_RampOrNormal = state;
}
//PID构造函数2，无斜坡版本
cPID::cPID(float kp,float ki,float kd ,float errAllMax,float outMax,float outStep,float precision,ePID_RampState state){
	memset(&PID.Kp1,0,sizeof(sGeneralPID));//全部初始为零
    PID.Kp1 = kp;
    PID.Ki1 = ki;
	PID.Kd1 = kd;
    PID.ErrAllMax = errAllMax;
    PID.OutMax 	= outMax;
    PID.OutStep	= outStep;
    PID.Precision = precision;
    PID.State_RampOrNormal = state;	  
} 

//构造函数 pid结构体作为参数
cPID::cPID(sGeneralPID copy){

	memset(&PID.Kp1,0,sizeof(sGeneralPID));//全部初始为零
	PID = copy;
}
cPID::cPID(void){
	memset(&PID.Kp1,0,sizeof(sGeneralPID));//全部初始为零
}

void cPID::updateParam(float NowInput)
{//更新PID的数据，即更新PID的输入值
	PID.Input = NowInput;
	if(PID.State_RampOrNormal == Ramp_e)
	{
		if(PID.RampCountTime < PID.RampTartgetTime)
		{
			++ PID.RampCountTime;
		}
		else
		{
			PID.RampCountTime = 0;
			if(PID.Target <  PID.RampTarget )
			{//斜坡函数还没计数完，
				PID.Target += PID.RampTartgetStep;
				if(PID.Target >= PID.RampTarget)
				{
					PID.Target = PID.RampTarget;
					PID.State_RampOrNormal = Normal_e;
				}
			}
			else if(PID.Target >  PID.RampTarget)
			{
				PID.Target -= PID.RampTartgetStep;
				if(PID.Target <= PID.RampTarget)
				{
					PID.Target = PID.RampTarget;
					PID.State_RampOrNormal = Normal_e;
				}
			}
			else
			{//斜坡函数计数已经完成，要退出斜坡函数模式
				PID.State_RampOrNormal = Normal_e;
			}
		}
	}	
	PID.Err_lastlast = PID.Err_last;
	PID.Err_last = PID.Err_now;
	PID.Err_now = PID.Target - NowInput;
	
	
	if( PID.Err_now < PID.Precision && PID.Err_now > -PID.Precision )
		PID.Err_now = 0;
	
	
	PID.Err_all += PID.Err_now;
	
	if( PID.Err_all > PID.ErrAllMax)
	{
		PID.Err_all = PID.ErrAllMax;
	}
	else if( PID.Err_all < -PID.ErrAllMax)
	{
		PID.Err_all = -PID.ErrAllMax;
	}		
}

float cPID::calculate(){
	//位置式PID的离散计算公式
    PID.Out = PID.Kp1 * PID.Err_now + PID.Kd1 * (PID.Err_now - PID.Err_last);
	PID.Out += ( PID.Err_all * PID.Ki1 );
    
    //输出限幅
    if(PID.Out >= PID.OutMax)
    	PID.Out = PID.OutMax;
    if(PID.Out <= -PID.OutMax)
    	PID.Out = -PID.OutMax;
    
    //输出微分限幅
    if(PID.Out - PID.lastout > PID.OutStep)
    {
    	PID.Out = PID.lastout + PID.OutStep;
    }
    if(PID.Out - PID.lastout < -PID.OutStep)
    {
    	PID.Out = PID.lastout + -PID.OutStep;
    }
    //传递参数
    PID.lastout = PID.Out;
    //输出out
    return PID.Out;
} 
void cPID::setTargetWithoutRamp(float tar){
	PID.Target = tar;
	PID.State_RampOrNormal = Normal_e;//PID状态
}
void cPID::setTargetWithRamp(float tar){
	PID.RampTarget = tar;
	PID.State_RampOrNormal = Ramp_e;
}
float my_fabs(float num){
	return (num<0 ? -num : num);
}

float cPID::getTarget(void){
	return PID.Target;
}

sGeneralPID& cPID::getPIDStruct(void){
		return PID;
}


















