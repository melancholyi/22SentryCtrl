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

struct sJudegeMsg{
    uint8_t gameProgress;    ////0是未开始比赛，1是准备阶段，2是自检阶段，3是5S倒计时，4是对战中，5是比赛结算中
    uint8_t fireSpeed;    // fric speed is overHeat
    uint8_t fireFreq;       //rammer speed
    uint16_t remainBullutNum  ; 
    uint16_t bullutSpeed;  //
    uint16_t chassisSpeed;

    uint8_t  enemyColor;      //enemy color
    uint16_t enemyHP_Hero;
    uint16_t enemyHP_infantry3;
    uint16_t enemyHP_infantry4;
    uint16_t enemyHP_infantry5;
    uint16_t myOutPostHP;
    sJudegeMsg(){
        memset(&gameProgress,0,sizeof(sJudegeMsg));	
    } 
};

void sendJudgeMSGtoGimbal(void);
void progMsgRecFromGM(CanRxMsg *msg);



struct sDbusMsgFromTop{
    int8_t s1;
    int8_t s2;
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    sDbusMsgFromTop(){
        s1 = s2 = 1 ; 
		ch0 = ch1 = ch2 = ch3= 1024;
    }
};
sDbusMsgFromTop getDbusMsgFromCanCom(void);

#endif // !__GM_2_CH_H
