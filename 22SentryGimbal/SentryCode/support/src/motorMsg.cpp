#include "motorMsg.h"

//���캯��
cMotorMsg::cMotorMsg(eGMWhichMotor whichApp , eCanX whichCan ,eCanID whichID,eMotorType mType){
	info.apply =whichApp;
	info.can = whichCan;
	info.id = whichID;
	info.type = mType;
	if(info.type == M3508_e){
		info.MAXCur = MaxCur_M3508_e;
		info.callbackID = M3508_RecBaseID_e + info.id;
		if(info.id>=one_e && info.id<=four_e)
			info.sendID = M3508_SendID_1_4_e ;
		if(info.id>=five_e && info.id<=eigth_e)
			info.sendID = M3508_SendID_5_8_e ;
	}
	else if(info.type == M2006_e){
		info.MAXCur = MaxCur_M2006_e;
		info.callbackID = M2006_RecBaseID_e + info.id;
		if(info.id>=one_e && info.id<=four_e)
			info.sendID = M2006_SendID_1_4_e ;
		if(info.id>=five_e && info.id<=eigth_e)
			info.sendID = M2006_SendID_5_8_e ;
	}
	else if(info.type == M6020_e){
		info.MAXCur = MaxCur_M6020_e;
		info.callbackID = M6020_RecBaseID_e + info.id;
		if(info.id>=one_e && info.id<=four_e)
			info.sendID = M6020_SendID_1_4_e ;
		if(info.id>=five_e && info.id<=eigth_e)
			info.sendID = M6020_SendID_5_7_e ;
	}	 
	memset(&motor_msg,0,sizeof(sMotorMsg)); 
	memset(&allAngelHelp,0,sizeof(sGetAngelAll));  
	//��������ʵ����ṹ�����ʱ���Ѿ���ʼ�����ˣ���Ϊ��ȫ��ѧϰC++����д����
}



//private:�Ƕ�����������
const float ContinueAngelMax =132.0;//�Ƕ��������ȽϽǶ�
float cMotorMsg::getContinueAngel(float last ,float now){

	float Incre = 0;
	Incre = now - last;
	if(Incre >= 0 && Incre < ContinueAngelMax){//��ת
		Incre += 0;
		return Incre;}
	if(Incre < -ContinueAngelMax){//��ת�����Ҳ�����
		Incre += 360.0;
		return Incre;
	}
	if(Incre <= 0 && Incre > -ContinueAngelMax){//��ת
		Incre += 0;
		return Incre;
	}
	if(Incre > ContinueAngelMax){//��ת�����Ҳ�����
		Incre -= 360.0;
		return Incre;
	}
	return  Incre;
}

//public:ת���Ƕ��ۼ�
void cMotorMsg::allAngelRecoderLoop(void){
	allAngelHelp.lastAngel = allAngelHelp.nowAngel;
	allAngelHelp.nowAngel = motor_msg.angel;
	allAngelHelp.increAngel  = getContinueAngel(allAngelHelp.lastAngel,allAngelHelp.nowAngel);
	allAngelHelp.allAngel +=  allAngelHelp.increAngel;
	motor_msg.angelAll = allAngelHelp.allAngel;
}

//public:�ⲿ�ӿں��� const ����Ա�����������޸ĳ�Ա����
sMotorMsg* cMotorMsg::getMotorMSG(void) {
	return &motor_msg;
}
//public:�ⲿ�ӿں��� const ����Ա�����������޸ĳ�Ա����
sApplyInfo* cMotorMsg::getApplyInfo(void) {
	return &info;
}

void cMotorMsg::setMotorMsg(sMotorMsg copy){
	memcpy(&motor_msg,&copy,sizeof(sMotorMsg));
}





