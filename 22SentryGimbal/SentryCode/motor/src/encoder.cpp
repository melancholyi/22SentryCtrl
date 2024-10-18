#include "encoder.h"
#include "motorMsg.h"
#include "can.h"
#include <vector>
#include <cstdio>
#include "gm2ch.h"
#include "imu.h"

//初始化 这个顺序必须严格执行

cMotorMsg GimbalAllMotorMsg[5] ={
//条件编译 为了让上下云台用一套工程 从而只需要修改此全局宏定义即可
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


//arr[0]~arr[4]分别为yaw pitch rammer F0 F1 
//结合枚举eGMWhichMotor实现


extern "C" void CAN1_RX0_IRQHandler(void){
	// printf("in\n");
	
	CanRxMsg RxMessage;
	static sMotorMsg tempMsg;
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!=RESET){
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);  

		//保存当前反馈的一帧数据
		
		tempMsg.angel 		= (float)(RxMessage.Data[0] << 8 | RxMessage.Data[1])*360.0/8192.0; //0 ~ 360.0
		tempMsg.speed 		= RxMessage.Data[2] << 8 | RxMessage.Data[3];//prm
		tempMsg.torque 		= RxMessage.Data[4] << 8 | RxMessage.Data[5];//转矩
		 tempMsg.temperature 	= RxMessage.Data[6];//温度
		tempMsg.null 	= RxMessage.Data[7];
		//根据ID来实例化对象
		//PITCH
		if(RxMessage.StdId == GimbalAllMotorMsg[GMPitch_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[GMPitch_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMPitch_e].allAngelRecoderLoop();
			return;
		}	

		//摩擦轮0
		else if(RxMessage.StdId == GimbalAllMotorMsg[GMFriction0_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[GMFriction0_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMFriction0_e].allAngelRecoderLoop();
			return ;
		}
		//摩擦轮1	
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
		//根据ID设置点击信息
		//YAW encoder 
		//保存当前反馈的一帧数据
		tempMsg.angel 	= (float)(RxMessage.Data[0] << 8 | RxMessage.Data[1])*360.0/8192.0; //0 ~ 360.0
		tempMsg.speed 	= RxMessage.Data[2] << 8 | RxMessage.Data[3];//prm
		tempMsg.torque 	= RxMessage.Data[4] << 8 | RxMessage.Data[5];//转矩
		tempMsg.temperature = RxMessage.Data[6];//温度
		tempMsg.null 	= RxMessage.Data[7];
		
		if(RxMessage.StdId == GimbalAllMotorMsg[GMYAW_e].getApplyInfo()->callbackID){
			GimbalAllMotorMsg[GMYAW_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMYAW_e].allAngelRecoderLoop();
			return ;
		}
		//rammer拨弹轮
		else if(RxMessage.StdId == GimbalAllMotorMsg[GMRammer_e].getApplyInfo()->callbackID){	
			//tempMsg.speed /= 36;
			GimbalAllMotorMsg[GMRammer_e].setMotorMsg(tempMsg);
			GimbalAllMotorMsg[GMRammer_e].allAngelRecoderLoop();
			return ;
		}
//		//云台通信 上下云台写到了一起 通过 ID 区分
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
