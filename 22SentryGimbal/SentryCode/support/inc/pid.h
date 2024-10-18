#ifndef __PID__H
#define __PID__H

#include "sys.h"

typedef enum 
{
	Normal_e = 0,				//PID����������״̬
	Ramp_e = 1,					//PID������б�º���״̬
	Ramping_Add = 2,		//б�»�û��ɣ������Ǽ�
	Ramping_Reduce = 3,	//б�»�û��ɣ������Ǽ�
}ePID_RampState;


struct sGeneralPID
{
	float Kp1;								//P				
	float Ki1;								//I
	float Kd1;								//D
	
	float Err_now;				//��ǰ���
	float Err_last;				//��һ�����
	float Err_lastlast;		//���ϴ����
	float Err_all;				//������
	float Target;					//Ŀ��
	float Input;					//����
	float Out;						//���
	float lastout;				//��һ�����
  
	uint8_t State_RampOrNormal;//PID״̬
	
	float RampTarget;					 //б�º�����Ŀ��
	float RampCountTime;			 //б�¼������� Target���Ƚ��������Ƿ�����һ������
	float RampTartgetTime;		 //�������ٴβŰ�count����1������	
	float RampTartgetStep;		 //б�º�������
	
	float ErrAllMax;			//PID�����޷�
	float OutMax;					//PID����޷�
	float OutStep;				//PID�����������
		
	float Precision;			//PID��С����
	//���캯��
	sGeneralPID(){//��ʼ��Ϊ��
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
		
	//���캯��1,Ŀ����б�°汾
	cPID(float kp,float ki,float kd , float errAllMax,float outMax,float outStep,float precision,ePID_RampState state,float rampStep);
	//���캯��2��Ŀ����б�°汾
	cPID(float kp,float ki,float kd , float errAllMax,float outMax,float outStep,float precision,ePID_RampState state);
	//���캯�� pid�ṹ����Ϊ����
	cPID(sGeneralPID copy);
	cPID(void);
	



	//����PID�����ݣ�������PID������ֵ
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