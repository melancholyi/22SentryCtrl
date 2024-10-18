#include "ch2gm.h"
#include "judge.h"
#include "stm32f4xx_can.h"

#define HEAT_LOW	100
#define HEAT_MID	200
#define HEAT_HIGH	250
#define HEAT_VERY_HIGH 300

int i = -1;
void sendJudgeMSGtoGimbal(void){
	i++;
    //1 chassis速度
	int16_t nowSpeed =0; // getChassisVelocity(1);
	uint16_t now_heat_top=0,now_heat_bottom,maxHeat_top,maxHeat_bottom;
	uint16_t heroHP,infantry3HP,infantry4HP,infantry5HP,outpostHP;
	float bulletV_top,bulletV_bottom;
	uint16_t bulletV_top_u16,bulletV_bottom_u16;
	uint8_t bulletFreq_top,bulletFreq_bottom;

	//2 取热量
	now_heat_top = (*Judge_GetRobot_power_heat()).shooter_id1_17mm_cooling_heat;
	now_heat_bottom = (*Judge_GetRobot_power_heat()).shooter_id2_17mm_cooling_heat;
	maxHeat_top = (*Judge_robot_status()).shooter_id1_17mm_cooling_limit;
	maxHeat_bottom = (*Judge_robot_status()).shooter_id2_17mm_cooling_limit;
	
	//3 取射速 and fireFreq，精度为小数点后面三的位，转化为16位无符号 
	if(Judge_shoot_data()->shooter_id == 1){//top
		bulletV_top = Judge_shoot_data()->bullet_speed;
		bulletV_top_u16 = (uint16_t)(bulletV_top*1000);
		bulletFreq_top = Judge_shoot_data()->bullet_freq;
	}
	else if (Judge_shoot_data()->shooter_id == 2){
		bulletV_bottom = Judge_shoot_data()->bullet_speed;
		bulletV_bottom_u16 = (uint16_t)(bulletV_bottom*1000);
		bulletFreq_bottom =Judge_shoot_data()->bullet_freq;
	}
	else ; //42mm bullut
	//printf("JUDGE:%d,%d,%.2f,%.2f\n",now_heat_top,now_heat_bottom,bulletV_top,bulletV_bottom);
	
	//4 取剩余弹量
	int16_t remaining_bullet=JudgeRemainBulletNum()->bullet_remaining_num_17mm;
	
	//5 取游戏状态
	uint8_t gameState = Judge_Game_State().game_progress;
	//6 取机器人颜色
    uint8_t RobotColor = 2;
	if((Judge_Robot_State()).robot_id == RedSentry)
			RobotColor=0;													//红色
	else if((Judge_Robot_State()).robot_id == BlueSentry)
			RobotColor=1;													//蓝色
	else
			RobotColor=2;													//出错

	//7 get enemy robot HP
    if((Judge_Robot_State()).robot_id<=RedRadar){
		heroHP = (Judge_Robot_HP().blue_1_robot_HP) / 5;
		infantry3HP = (Judge_Robot_HP().blue_3_robot_HP) / 5;
		infantry4HP = (Judge_Robot_HP().blue_4_robot_HP) / 5;
		infantry5HP = (Judge_Robot_HP().blue_5_robot_HP) / 5;
		outpostHP = (Judge_Robot_HP().red_outpost_HP) / 10;
	}
	else if((Judge_Robot_State()).robot_id>=BlueHero){
		heroHP = (Judge_Robot_HP().red_1_robot_HP) / 5;
		infantry3HP = (Judge_Robot_HP().red_3_robot_HP) / 5;
		infantry4HP = (Judge_Robot_HP().red_4_robot_HP) / 5;
		infantry5HP = (Judge_Robot_HP().red_5_robot_HP) / 5;
		outpostHP = (Judge_Robot_HP().blue_outpost_HP) / 10;
	}

	if(i%4 == 0){
		//load data to can frame
		CanTxMsg TxMsgtoTop;
		TxMsgtoTop.StdId = CH2GM_CTRL_MSG_TOP_CAN_ID_e;
		TxMsgtoTop.RTR = CAN_RTR_DATA;
		TxMsgtoTop.IDE = CAN_Id_Standard;
		TxMsgtoTop.DLC = 0x08;

		TxMsgtoTop.Data[0]=(uint8_t)RobotColor;	

		//Data[1] 比赛进程
		TxMsgtoTop.Data[1] = gameState;

		//Data[2] and Data[3] remain bullut
		TxMsgtoTop.Data[2]=(uint8_t)(remaining_bullet>>8);		//剩余弹量高八位
		TxMsgtoTop.Data[3]=(uint8_t)(remaining_bullet);			//剩余弹量低八位

		//Data[4] and Data[5] shoot speed Fric Speed
		TxMsgtoTop.Data[4]=(uint8_t)(bulletV_top_u16>>8);	//射速高八位
		TxMsgtoTop.Data[5]=(uint8_t)(bulletV_top_u16);	    //射速低八位

		////Data[6] and Data[7] now chassis move speed
		TxMsgtoTop.Data[6] = (uint8_t)(nowSpeed) ;			//速度高八位
		TxMsgtoTop.Data[7] = (uint8_t)(nowSpeed >> 8) ;		//速度di八位			
		CAN_Transmit(CAN1, &TxMsgtoTop);		
	}
	else if(i%4 == 1){
		//发送血量
		CanTxMsg TxMsgtoTop;
		TxMsgtoTop.StdId = CH2GM_HP_MSG_TOP_CAN_ID_e;
		TxMsgtoTop.RTR = CAN_RTR_DATA;
		TxMsgtoTop.IDE = CAN_Id_Standard;
		TxMsgtoTop.DLC = 0x06;
		memset(TxMsgtoTop.Data,0,sizeof(char)*8);
		TxMsgtoTop.Data[0] = (uint8_t)heroHP;
		TxMsgtoTop.Data[1] = (uint8_t)infantry3HP ;
		TxMsgtoTop.Data[2] = (uint8_t)infantry4HP ;
		TxMsgtoTop.Data[3] = (uint8_t)infantry5HP ;	
		TxMsgtoTop.Data[4] = (uint8_t)outpostHP;	
		TxMsgtoTop.Data[5] = (uint8_t)(now_heat_top/5);
		CAN_Transmit(CAN1, &TxMsgtoTop);
	}

    
	
		
	
/********************* BOTTOM *************************/
	if(i%4 == 2){
		//load data to can frame
		CanTxMsg TxMsgtoBottom;
		TxMsgtoBottom.StdId = CH2GM_CTRL_MSG_BOTTOM_CAN_ID_e;
		TxMsgtoBottom.RTR = CAN_RTR_DATA;
		TxMsgtoBottom.IDE = CAN_Id_Standard;
		TxMsgtoBottom.DLC = 0x08;
		//Data[0] 颜色
		TxMsgtoBottom.Data[0]=(uint8_t)RobotColor;	
		//printf("Rcolor:%d\n",RobotColor);
		//Data[1] 比赛进程
		TxMsgtoBottom.Data[1]=(uint8_t)gameState;	
		
		//Data[2] and Data[3] remain bullut
		TxMsgtoBottom.Data[2]=(uint8_t)(remaining_bullet>>8);		//剩余弹量高八位
		TxMsgtoBottom.Data[3]=(uint8_t)(remaining_bullet);			//剩余弹量低八位

		//Data[4] and Data[5] shoot speed Fric Speed
		TxMsgtoBottom.Data[4]=(uint8_t)(bulletV_bottom_u16>>8);	//射速高八位
		TxMsgtoBottom.Data[5]=(uint8_t)(bulletV_bottom_u16);	    //射速低八位

		////Data[6] and Data[7] now chassis move speed
		TxMsgtoBottom.Data[6] = (uint8_t)(nowSpeed) ;			//速度高八位
		TxMsgtoBottom.Data[7] = (uint8_t)(nowSpeed >> 8) ;		//速度di八位	
				
		if((Judge_Robot_State()).robot_id == BlueSentry || (Judge_Robot_State()).robot_id == RedSentry)
				CAN_Transmit(CAN1, &TxMsgtoBottom);	
	}

    if(i%4 == 3){
		//发送血量
		CanTxMsg TxMsgtoBottom;
		TxMsgtoBottom.StdId = CH2GM_HP_MSG_BOTTOM_CAN_ID_e;
		TxMsgtoBottom.RTR = CAN_RTR_DATA;
		TxMsgtoBottom.IDE = CAN_Id_Standard;
		TxMsgtoBottom.DLC = 0x06;
		memset(TxMsgtoBottom.Data,0,sizeof(char)*8);
		TxMsgtoBottom.Data[0] = (uint8_t)heroHP;
		TxMsgtoBottom.Data[1] = (uint8_t)infantry3HP ;
		TxMsgtoBottom.Data[2] = (uint8_t)infantry4HP ;
		TxMsgtoBottom.Data[3] = (uint8_t)infantry5HP ;	
		TxMsgtoBottom.Data[4] = (uint8_t)outpostHP;	
		TxMsgtoBottom.Data[5] = (uint8_t)(now_heat_bottom/5);	
		CAN_Transmit(CAN1, &TxMsgtoBottom);
	}
	
}


sDbusMsgFromTop DbusMsg;

sDbusMsgFromTop getDbusMsgFromCanCom(void){
    return DbusMsg;
}
void progMsgRecFromGM(CanRxMsg *msg){
	//printf("in\n");
	if(msg->StdId == DBUS_CTRL_TOP2CH){
        DbusMsg.s1 =  msg->Data[0];
        DbusMsg.ch0 = msg->Data[1]<<8|msg->Data[2]  ;
        return ;
    }

}






