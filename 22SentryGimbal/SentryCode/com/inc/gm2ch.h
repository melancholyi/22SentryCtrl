#ifndef __GM_2_CH_H
#define __GM_2_CH_H

#include "stm32f4xx_can.h"

#include "stdint.h"
#include <cstring>
enum eGMCH_ComStdID{
    GM2CH_TOP_CAN_ID_e      = 0x111,
    GM2CH_BOTTOM_CAN_ID_e   = 0x222,

    CH2GM_HP_MSG_TOP_CAN_ID_e   = 0x333,
    CH2GM_CTRL_MSG_TOP_CAN_ID_e         = 0x444,

    CH2GM_HP_MSG_BOTTOM_CAN_ID_e   = 0x555,
    CH2GM_CTRL_MSG_BOTTOM_CAN_ID_e   = 0x666,

    CH2CH_T2B_CAN_ID_e  = 0x777,
    CH2CH_B2T_CAN_ID_e  = 0x387,

    DBUS_CTRL_TOP2CH = 0X167,
    DBUS_CTRL_TOP2BOTTOM = 0X357
};

enum eShootHeatLevel{
    HEAT_LOW = 0,
    HEAT_MID ,
    HEAT_HIGH,
    HEAT_VERY_HIGH
};

struct sJudegeMsg{
    uint8_t gameState;    ////0是未开始比赛，1是准备阶段，2是自检阶段，3是5S倒计时，4是对战中，5是比赛结算中
    uint8_t fireSpeed;    // fric speed is overHeat
    uint8_t fireFreq;       //rammer speed
    uint16_t remainBullutNum  ; 
    float bullutSpeed;  //
    uint16_t chassisSpeed;
    eShootHeatLevel   shootHeat_Lv;

    uint8_t  myColor;      
    uint16_t enemyHP_Hero;
    uint16_t enemyHP_infantry3;
    uint16_t enemyHP_infantry4;
    uint16_t enemyHP_infantry5;
    uint16_t myOutPostHP;
	uint16_t shoot_heat;
    sJudegeMsg(){
        memset(&gameState,0,sizeof(sJudegeMsg));
        shootHeat_Lv = HEAT_MID;
    } 
};

//1、接口函数 上下位机都使用 获取裁判系统信息
sJudegeMsg* getJudgeMsgFromCan(void);
void judgeMsgProcess(CanRxMsg * msg);

#if Gimbal_TOP_DEFINE  //TBSYCY
//2、TOP 发送遥控器控制的函数
void TopGimSendDbusMsg2OthersViaCan2(void);
#endif 
 
#if Gimbal_BOTTOM_DEFINE //TBSYCY
//3 、bottom slove can pack from top
struct sDbusMsgFromTop{
    int8_t s1;
    int8_t s2;
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    sDbusMsgFromTop(){
        s1 = s2 = ch0 = ch1 = ch2 = ch3= 0;
    }
};
sDbusMsgFromTop& getDbusMsgFromCanCom(void);
void DbusCanProgress(CanRxMsg &msg);

#endif


//4 



#endif // !__GM_2_CH_H
