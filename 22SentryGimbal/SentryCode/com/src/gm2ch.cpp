#include "gm2ch.h"
#include "stm32f4xx_can.h"
#include "dbus.h"
sJudegeMsg judge_msg;


//云台处理地盘发送的裁判系统信息
void judgeMsgProcess(CanRxMsg * msg){ //process the  msg  received from ch
#if Gimbal_TOP_DEFINE  //TBSYCY2
    if(msg->StdId == CH2GM_CTRL_MSG_TOP_CAN_ID_e){
        //1 get color
       judge_msg.myColor = msg->Data[0] ;
		printf("A:%d\n",judge_msg.myColor);
        //1 get isBeginGame
        judge_msg.gameState = msg->Data[1] ;

        //2 3 获取剩余子弹
        judge_msg.remainBullutNum = msg->Data[2]<<8|msg->Data[3];	
        //4 5 获取射速
        judge_msg.bullutSpeed =float(msg->Data[4]<<8|msg->Data[5])/1000;
        //6 7 获取chassis speed		
        judge_msg.chassisSpeed = msg->Data[6]<<8|msg->Data[7];		
        return ;
    }
    if(msg->StdId == CH2GM_HP_MSG_TOP_CAN_ID_e){
        judge_msg.enemyHP_Hero = msg->Data[0]*5;
        judge_msg.enemyHP_infantry3 = msg->Data[1]*5;
        judge_msg.enemyHP_infantry4 = msg->Data[2]*5; 
        judge_msg.enemyHP_infantry5 = msg->Data[3]*5;
        judge_msg.myOutPostHP = msg->Data[4]*10;
		judge_msg.shoot_heat = msg->Data[5]*5;
        return ;
    }

#elif Gimbal_BOTTOM_DEFINE
    if(msg->StdId == CH2GM_CTRL_MSG_BOTTOM_CAN_ID_e){
         //1 get color
       judge_msg.myColor = msg->Data[0] ;
		//printf("color:%d\n",judge_msg.myColor);
        judge_msg.gameState = msg->Data[1]	;
		printf("state:%d\n",judge_msg.gameState);
        //2 3 获取剩余子弹
        judge_msg.remainBullutNum = msg->Data[2]<<8|msg->Data[3];	
        //4 5 获取射速
        judge_msg.bullutSpeed = (msg->Data[4]<<8|msg->Data[5])/1000;	
        //6 7 获取chassis speed	
        judge_msg.chassisSpeed = msg->Data[6]<<8|msg->Data[7];		
        return ;
    }
    if(msg->StdId == CH2GM_HP_MSG_BOTTOM_CAN_ID_e){
        judge_msg.enemyHP_Hero = msg->Data[0]*5;
        judge_msg.enemyHP_infantry3 = msg->Data[1]*5;
        judge_msg.enemyHP_infantry4 = msg->Data[2]*5; 
        judge_msg.enemyHP_infantry5 = msg->Data[3]*5;
        judge_msg.myOutPostHP = msg->Data[4]*10;
		judge_msg.shoot_heat = msg->Data[5]*5;
		//printf("heat:%d\n",judge_msg.shoot_heat);
        return ;
    }
#endif 
} 


//1、接口函数 上下位机都使用 获取裁判系统信息
sJudegeMsg* getJudgeMsgFromCan(void){
    return &judge_msg;
}



////////////////////////////////////////////////////////////////////
////////////////////////// 发送函数 ////////////////////////////////
////////////////////////////////////////////////////////////////////
#if Gimbal_TOP_DEFINE  //TBSYCY
//TOP can发送函数
uint64_t times;
static RC_CtrlTypeDef* dbus_msg;
void TopGimSendDbusMsg2OthersViaCan2(void){
	dbus_msg = getRCInfo();
	times++;
	if(times % 2 == 0){
		CanTxMsg TxMsg = {0};
		TxMsg.StdId = DBUS_CTRL_TOP2CH;
		TxMsg.RTR = CAN_RTR_DATA;
		TxMsg.IDE = CAN_Id_Standard;
		TxMsg.DLC = 3;
		TxMsg.Data[0] =  dbus_msg->rc.s1 ;
		TxMsg.Data[1] =  (int16_t)dbus_msg->rc.ch0 >> 8;
		TxMsg.Data[2] = (int16_t)dbus_msg->rc.ch0 & 0x00ff;
		CAN_Transmit(CAN2, &TxMsg);
	}
	else if(times % 2 == 1){
		CanTxMsg TxMsg = {0};
		TxMsg.StdId = DBUS_CTRL_TOP2BOTTOM;
		TxMsg.RTR = CAN_RTR_DATA;
		TxMsg.IDE = CAN_Id_Standard;
		TxMsg.DLC = 8;
		TxMsg.Data[0] =  dbus_msg->rc.s1 ;
		TxMsg.Data[1] =  dbus_msg->rc.s2 ;
		TxMsg.Data[2] =  (int16_t)dbus_msg->rc.ch2 >> 8;
		TxMsg.Data[3] = (int16_t)dbus_msg->rc.ch2 & 0x00ff;
		TxMsg.Data[4] =  (int16_t)dbus_msg->rc.ch3 >> 8;
		TxMsg.Data[5] = (int16_t)dbus_msg->rc.ch3 & 0x00ff;
		TxMsg.Data[6] =  (int16_t)dbus_msg->rc.ch1 >> 8;
		TxMsg.Data[7] = (int16_t)dbus_msg->rc.ch1 & 0x00ff;
		CAN_Transmit(CAN2, &TxMsg);
	}
}
#endif 


//bottom slove can pack from top 
#if Gimbal_BOTTOM_DEFINE //TBSYCY
sDbusMsgFromTop DbusMsg;
sDbusMsgFromTop& getDbusMsgFromCanCom(void){
    return DbusMsg;
}

void DbusCanProgress(CanRxMsg &msg){
    if(msg.StdId == DBUS_CTRL_TOP2BOTTOM){
        DbusMsg.s1 = msg.Data[0];
        DbusMsg.s2 =  msg.Data[1];
        DbusMsg.ch2 = msg.Data[2]<<8|msg.Data[3]  ;
        DbusMsg.ch3 = msg.Data[4]<<8|msg.Data[5]  ;
        DbusMsg.ch1 = msg.Data[6]<<8|msg.Data[7]  ;
        return ;
    }
}



#endif







