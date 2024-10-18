#include "encoder.h"
#include "motorMsg.h"
#include "can.h"
#include <vector>
#include <cstdio>
#include "gm2ch.h"
#include "imu.h"

//��ʼ�� ���˳������ϸ�ִ��

cMotorMsg GimbalAllMotorMsg[5] ={
//�������� Ϊ����������̨��һ�׹��� �Ӷ�ֻ��Ҫ�޸Ĵ�ȫ�ֺ궨�弴��
#if Gimbal_TOP_DEFINE  //TBSYCY1
	{GMYAW_e , CAN2_e , one_e , M6020_e},
#elif  Gimbal_BOTTOM_DEFINE 
	{GMYAW_e , CAN2_e , six_e , M6020_e},
#endif
	{GMPitch_e , CAN1_e , one_e , M6020_e},  
	{GMRammer_e , CAN2_e , six_e ,M2006_e},
	{GMFriction0_e , CAN1_e , seven_e ,M3508_e},
	{GMFriction1_e , CAN1_e , six_e ,M3508_e}	
	
};


//arr[0]~arr[4]�ֱ�Ϊyaw pitch rammer F0 F1 
//���ö��eGMWhichMotorʵ��


extern "C" void CAN1_RX0_IRQHandler(void){
	// printf("in\n");
	
	CanRxMsg RxMessage;
	static sMotorMsg tempMsg;
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!=RESET){
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);  

		//���浱ǰ������һ֡����
		
		tempMsg.angel 		= (float)(RxMessage.Data[0] << 8 | RxMessage.Data[1])*360.0/8192.0; //0 ~ 360.0
		tempMsg.speed 		= RxMessage.Data[2] << 8 | RxMessage.Data[3];//prm
		tempMsg.torque 		= RxMessage.Data[4] << 8 | RxMessage.Data[5];//ת��
		 tempMsg.temperature 	= RxMessage.Data[6];//�¶�
		tempMsg.null 	= RxMessage.Data[7];
		//����ID��ʵ��������
		//PITCH
		if(RxMessage.StdId == GimbalAllMotorMsg[GMPitch_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[GMPitch_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMPitch_e].allAngelRecoderLoop();
			return;
		}	

		//Ħ����0
		else if(RxMessage.StdId == GimbalAllMotorMsg[GMFriction0_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[GMFriction0_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMFriction0_e].allAngelRecoderLoop();
			return ;
		}
		//Ħ����1	
		else if(RxMessage.StdId == GimbalAllMotorMsg[GMFriction1_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[GMFriction1_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMFriction1_e].allAngelRecoderLoop();
			return ;
		}	
	}
}

extern "C"  void CAN2_RX0_IRQHandler(void) {
	//printf("in\n");
	CanRxMsg RxMessage ={0};
	sMotorMsg tempMsg ;
	if(CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET){
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);  
		//����ID���õ����Ϣ
		//YAW encoder 
		//���浱ǰ������һ֡����
		tempMsg.angel 	= (float)(RxMessage.Data[0] << 8 | RxMessage.Data[1])*360.0/8192.0; //0 ~ 360.0
		tempMsg.speed 	= RxMessage.Data[2] << 8 | RxMessage.Data[3];//prm
		tempMsg.torque 	= RxMessage.Data[4] << 8 | RxMessage.Data[5];//ת��
		tempMsg.temperature = RxMessage.Data[6];//�¶�
		tempMsg.null 	= RxMessage.Data[7];
		
		if(RxMessage.StdId == GimbalAllMotorMsg[GMYAW_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[GMYAW_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMYAW_e].allAngelRecoderLoop();
			return ;
		}
		//rammer������
		else if(RxMessage.StdId == GimbalAllMotorMsg[GMRammer_e].getApplyInfo()->callbackID){	
			//tempMsg.speed /= 36;
			GimbalAllMotorMsg[GMRammer_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMRammer_e].allAngelRecoderLoop();
			return ;
		}
//		//��̨ͨ�� ������̨д����һ�� ͨ�� ID ����
//		else{
//			judgeMsgProcess(&RxMessage);
//			#if Gimbal_BOTTOM_DEFINE  // TBSYCY
//			DbusCanProgress(RxMessage);
//			#endif 
//			return;
//		}
	}
}

sMotorMsg* getMotorMsgFromCan(eGMWhichMotor whichMotor){
	return GimbalAllMotorMsg[whichMotor].getMotorMSG();
}

sApplyInfo* getMotorApplyInfo(eGMWhichMotor which){
	return GimbalAllMotorMsg[which].getApplyInfo();
}

void encoderPrint(eGMWhichMotor apps){
	printf("MSG:%.2f,%.2f,%d,%d,%d\n",
		getMotorMsgFromCan(apps)->angelAll,
		getMotorMsgFromCan(apps)->angel,
		getMotorMsgFromCan(apps)->speed,
		getMotorMsgFromCan(apps)->temperature,
		getMotorMsgFromCan(apps)->torque
	);  
}
