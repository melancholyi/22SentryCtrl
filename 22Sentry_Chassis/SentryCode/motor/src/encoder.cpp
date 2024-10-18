#include "encoder.h"
#include "motorMsg.h"
#include "can.h"
#include <vector>
#include <cstdio>
#include "ch2gm.h"


//��ʼ�� ���˳������ϸ���ö�����͵�ֵ ����CMDriving_e = 0,���Է��������һλ
cMotorMsg GimbalAllMotorMsg[]={
//chissis motor encoder
	{CMDriving_e , CAN2_e , one_e , M3508_e},
	{CMBrake_e, CAN2_e , two_e ,M2006_e}
};


//GM CH com.	
extern "C" void CAN1_RX0_IRQHandler(void){
	CanRxMsg RxMessage;
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!=RESET){
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);  
		//ch receive msg from gm. and then process
		progMsgRecFromGM(&RxMessage);
	}	
}


//chassis motor encoder
extern "C"  void CAN2_RX0_IRQHandler(void) {
	static CanRxMsg RxMessage = {0};
	if(CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET){
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
		//drive and brake motor encoder
		//���浱ǰ������һ֡����
		static sMotorMsg tempMsg ;
		tempMsg.angel 	= (RxMessage.Data[0] << 8 | RxMessage.Data[1])*360.0/8192.0; //0 ~ 360.0
		tempMsg.speed 	= RxMessage.Data[2] << 8 | RxMessage.Data[3];//prm
		tempMsg.torque 	= RxMessage.Data[4] << 8 | RxMessage.Data[5];//ת��
		tempMsg.temperature = RxMessage.Data[6];//�¶�
		tempMsg.null 	= RxMessage.Data[7];
		
		//drive encoder 
		if(RxMessage.StdId == GimbalAllMotorMsg[CMDriving_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[CMDriving_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[CMDriving_e].allAngelRecoderLoop();
			return ;
		}
		//brake encoder
		else if(RxMessage.StdId == GimbalAllMotorMsg[CMBrake_e].getApplyInfo()->callbackID){
			tempMsg.speed = tempMsg.speed ;
			GimbalAllMotorMsg[CMBrake_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[CMBrake_e].allAngelRecoderLoop();
			return ;
		}
}
}

sMotorMsg* getMotorMsgFromCan(eGMWhichMotor whichMotor){
	return GimbalAllMotorMsg[whichMotor].getMotorMSG();
}

sApplyInfo* getMotorApplyInfo(eGMWhichMotor which){
	return GimbalAllMotorMsg[which].getApplyInfo();
}

void encoderPrint(eGMWhichMotor app){
	printf("MSG:%.2f,%.2f,%d,%d,%d\n",
		getMotorMsgFromCan(app)->angelAll,
		getMotorMsgFromCan(app)->angel,
		getMotorMsgFromCan(app)->speed,
		getMotorMsgFromCan(app)->temperature,
		getMotorMsgFromCan(app)->torque
	);                                
}
