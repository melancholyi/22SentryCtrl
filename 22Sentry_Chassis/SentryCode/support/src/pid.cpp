#include "pid.h"

//PID���캯��1����б�°汾
 cPID::cPID(float kp,float ki,float kd,float errAllMax,float outMax,float outStep,float precision,ePID_RampState state,float rampStep){
	 memset(&PID.Kp1,0,sizeof(sGeneralPID));//ȫ����ʼΪ��
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
//PID���캯��2����б�°汾
cPID::cPID(float kp,float ki,float kd ,float errAllMax,float outMax,float outStep,float precision,ePID_RampState state){
	memset(&PID.Kp1,0,sizeof(sGeneralPID));//ȫ����ʼΪ��
    PID.Kp1 = kp;
    PID.Ki1 = ki;
	PID.Kd1 = kd;
    PID.ErrAllMax = errAllMax;
    PID.OutMax 	= outMax;
    PID.OutStep	= outStep;
    PID.Precision = precision;
    PID.State_RampOrNormal = state;	  
} 

//���캯�� pid�ṹ����Ϊ����
cPID::cPID(sGeneralPID copy){

	memset(&PID.Kp1,0,sizeof(sGeneralPID));//ȫ����ʼΪ��
	PID = copy;
}
cPID::cPID(void){
	memset(&PID.Kp1,0,sizeof(sGeneralPID));//ȫ����ʼΪ��
}

void cPID::updateParam(float NowInput)
{//����PID�����ݣ�������PID������ֵ
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
			{//б�º�����û�����꣬
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
			{//б�º��������Ѿ���ɣ�Ҫ�˳�б�º���ģʽ
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
	//λ��ʽPID����ɢ���㹫ʽ
    PID.Out = PID.Kp1 * PID.Err_now + PID.Kd1 * (PID.Err_now - PID.Err_last);
	PID.Out += ( PID.Err_all * PID.Ki1 );
    
    //����޷�
    if(PID.Out >= PID.OutMax)
    	PID.Out = PID.OutMax;
    if(PID.Out <= -PID.OutMax)
    	PID.Out = -PID.OutMax;
    
    //���΢���޷�
    if(PID.Out - PID.lastout > PID.OutStep)
    {
    	PID.Out = PID.lastout + PID.OutStep;
    }
    if(PID.Out - PID.lastout < -PID.OutStep)
    {
    	PID.Out = PID.lastout + -PID.OutStep;
    }
    //���ݲ���
    PID.lastout = PID.Out;
    //���out
    return PID.Out;
} 
void cPID::setTargetWithoutRamp(float tar){
	PID.Target = tar;
	PID.State_RampOrNormal = Normal_e;//PID״̬
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


















