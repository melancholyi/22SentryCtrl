#ifndef __DJI_MOTOR_MSG_H_
#define __DJI_MOTOR_MSG_H_

#include "stdint.h" //
#include <cstring>
#include <cstdio>

typedef enum{
	MaxCur_M3508_e = 16384,
	MaxCur_M2006_e = 8000,
	MaxCur_M6020_e = 30000
} eMotorMaxCur;

typedef enum{
	GMYAW_e   	= 0,
	GMPitch_e 	= 1,
  	GMRammer_e 	= 2 ,
	GMFriction0_e = 3,
	GMFriction1_e = 4,
	
	CMDriving_e = 0,
	CMBrake_e	=1,
	
	CM_LT_e = 0,
	CM_RT_e = 1,
	CM_LB_e = 2,
	CM_RB_e = 3
} eGMWhichMotor; //云台电机枚举

typedef enum{
	CAN1_e = 0,
	CAN2_e = 1,
} eCanX;

typedef enum{
	one_e = 1,
	two_e,
	three_e,
	four_e,
	five_e,
	six_e,
	seven_e,
	eigth_e
}eCanID;

typedef enum{
	M6020_e = 0,
	M3508_e  ,
	M2006_e 
}eMotorType;

typedef enum{
	M6020_SendID_1_4_e =	0x1FF,
	M6020_SendID_5_7_e =	0x2FF,
	M3508_SendID_1_4_e =	0x200,
	M3508_SendID_5_8_e	=	0x1FF,
	M2006_SendID_1_4_e =	0x200,
	M2006_SendID_5_8_e	=	0x1FF
}eCanSendBaseID;

typedef enum{
	M3508_RecBaseID_e  = 0x200,
	M2006_RecBaseID_e =  0x200,
	M6020_RecBaseID_e =  0x204
}eCanRecBaseID;

struct sApplyInfo{
	eGMWhichMotor apply;
	eCanX		can;
	eCanID		id; 
	eMotorType  type;
	uint32_t	sendID;
	uint32_t	callbackID;
	int16_t 	MAXCur;
};

struct sMotorMsg{ //C++ 定义结构体时不需要带struct前缀，所以不需要typedef
	float angelAll;
	float angel;   //说明：这里的这个角度单位是编码器返回的值
	int16_t speed;
	int16_t torque;
	int8_t  temperature;
	int8_t  null;
	sMotorMsg(){//C++的功能扩充，结构体的构造函数
		//定义结构体变量时，自动将结构体初始化为零
		memset(&angelAll,0,sizeof(sMotorMsg)); 
	}
} ;

struct sGetAngelAll{ 
	float nowAngel;
	float lastAngel;
	float increAngel;
	float allAngel;
	sGetAngelAll(){//C++的功能扩充，结构体的构造函数
		//定义结构体变量时，自动将结构体初始化为零
		memset(&nowAngel,0,sizeof(sGetAngelAll));  //<cstring>(c++) or "string.h"(c)库函数
	}
};


class cMotorMsg{
private:
	//类成员变量
	sApplyInfo info;
	sMotorMsg motor_msg;
	sGetAngelAll allAngelHelp;
	/********** 私有辅助成员函数 **********/
	float getContinueAngel(float last,float now);
public:
	//构造函数
	cMotorMsg(eGMWhichMotor whichApp , eCanX whichCan ,eCanID whichID , eMotorType mType);
	//角度连续化记录
	void allAngelRecoderLoop(void);
	sMotorMsg* getMotorMSG(void) ;
	sApplyInfo* getApplyInfo(void) ;
	
	void setMotorMsg(sMotorMsg copy);
};






#endif /*头文件预处理__MOTOR_MSG_H_*/