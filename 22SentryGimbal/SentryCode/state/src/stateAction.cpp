//#include "stateAction.h"
//#include "encoder.h"
//#include "GimabalTask.h"
//#include "VisionCom.h"
//#include "dbus.h"
//#include "gm2ch.h"
//#include "control.h"

///*********************** DBUS Param ************************/
//float yaw_target_angel = 0,pitch_target_angel=9;
//int16_t sendToChassisSpeed;


///*********************** scout Param ************************/
////scout
//#if Gimbal_TOP_DEFINE	//TBSYCY8
//    const static float  YAW_MIN_T    = TOP_GIMBAL_YAW_MIN;
//    const static float  YAW_MAX_T     = TOP_GIMBAL_YAW_MAX ;
//    const static float 	YAW_DELTA 	= 0.075;
//    
//    const static float  PITCH_MIN_T     = TOP_GIMBAL_PITCH_MIN; //top need to procrce
//    const static float  PITCH_MAX_T     = TOP_GIMBAL_PITCH_MAX;
//    const static float  PITCH_DELTA     = 0.045;
//	
//	//scout TarAngel
//	static float pitch_tar_angel = PITCH_MAX_T;
//	static float yaw_tar_angel = YAW_MAX_T;
//	
//	#define FRI0_DIR	1
//	#define FRI1_DIR	-1
//	#define RAMMER_POS_DIR -1
//	#define RAMMER_NEG_DIR 1
//	
//	int8_t down_flag = 1;
//	int8_t dir_flag = 0;
//	
//#elif Gimbal_BOTTOM_DEFINE
//    const static float  YAW_MIN_T    = -135;
//    const static float  YAW_MAX_T     =  1350;
//    const static float  YAW_DELTA = 0.105;
//    const static float  PITCH_MIN_T     = BOTTOM_GIMBAL_PITCH_MIN - 20;
//    const static float  PITCH_MAX_T     =  BOTTOM_GIMBAL_PITCH_MAX;
//    const static float  PITCH_DELTA = 0.0650;
//	//prepare angel
//	#define PITCH_ANGEL_PREPARE 	7
//	#define YAW_ANGEL_PREPARE		0
//	static float pitch_tar_angel = PITCH_ANGEL_PREPARE;
//	static float yaw_tar_angel = 0.0;
//	
//	#define FRI0_DIR	-1
//	#define FRI1_DIR	1
//	#define RAMMER_POS_DIR -1
//	#define RAMMER_NEG_DIR 1
//	int8_t down_flag = 0;
//	int8_t dir_flag = 1;
//	
//#endif




///*********************** AutoAim Param ************************/
//bool A2S_flag = true;
//bool S2A_flag = true;
//bool toDBUS_flag = true;

////autoaim
//const static float RAMMER_SPEED_LOW = 3000;
//const static float RAMMER_SPEED_MID = 6500;
//const static float RAMMER_SPEED_HIGH = 8500;
//const static float RAMMER_SPEED_INV = 800;

//const static float FRIC_SPEED_LOW = 2000;
//const static float FRIC_SPEED_MID = 4000;
//const static float FRIC_SPEED_HIGH = 7150;



////构造函数
//cStateAction::cStateAction(){
//    now_state_ = prepare_e;
//	last_state_ = now_state_;
//    fire_speed_ = fireSpeed_stop_e;
//    cool_time_ = 0; 
//    upper_cmd_ = NULL_SEND_MODE_e;
//    fire_freq_ = fireFreq_stop_e;
//    is_over_heat_ = false;
//    locked_rotor_ = 0;
//    reverse_time_ = 0;
//}

////set get val interface
//eState cStateAction::getNowState(void) const{
//    return now_state_;
//}
//void cStateAction::setState(eState state){
//    now_state_ =state;
//}
//void cStateAction::setFireLv(eFireLv lv){
//     fire_speed_ = lv;
//}
//void cStateAction::setFireFreq(eFireFreq freq){
//    fire_freq_ = freq;
//}

////ctrl loop
//void cStateAction::stateActionCtrlLoop(void){
//    //ycy!!! 发现暂时无用 stateSwitch();
//    if(now_state_ == prepare_e)
//        prepareStateAction();
//    else if(now_state_ == scout_e)
//        scoutStateAction();
//    else if(now_state_ == autoAim_e)
//        autoAimStateAction();
//    else if(now_state_ == dbus_e) 
//        dbusStateAction();
//    else return ;
//}

///*********************** Prepare ************************/
//void cStateAction::prepareStateAction(void){
//	GMCtrlSetTarget(GMYAW_e,getMotorMsgFromCan(GMYAW_e)->angelAll);
//	GMCtrlSetTarget(GMPitch_e,getMotorMsgFromCan(GMPitch_e)->angelAll);
//	GMCtrlSetTarget(GMFriction0_e , 0);
//	GMCtrlSetTarget(GMFriction1_e , 0);
//    GMCtrlSetTarget(GMRammer_e , 0);
//}



//void cStateAction::scoutStateAction(void){
//	if(last_state_ == dbus_e || last_state_ == autoAim_e || last_state_ == prepare_e){
//        last_state_ = now_state_;
//		yaw_tar_angel = getMotorMsgFromCan(GMYAW_e)->angelAll;
//		pitch_tar_angel = getMotorMsgFromCan(GMPitch_e)->angel;
//    }
//	
//	
//	
//	GMCtrlSetTarget(GMYAW_e,yaw_tar_angel);
//    GMCtrlSetTarget(GMPitch_e,pitch_tar_angel);
//	GMCtrlSetTarget(GMFriction0_e , 0);
//	GMCtrlSetTarget(GMFriction1_e , 0);
//	GMCtrlSetTarget(GMRammer_e , 0);
//   //!!!!!! error attention to write fun
//   //!!!!!!! yaw need to avoid 10888 ---> 180. avoid rolling crazyily
//    if(yaw_tar_angel >= YAW_MAX_T) dir_flag = 0;
//    if(yaw_tar_angel <= YAW_MIN_T) dir_flag = 1;
//    if(dir_flag) yaw_tar_angel += YAW_DELTA;
//    else yaw_tar_angel -= YAW_DELTA;

//    if(pitch_tar_angel >= PITCH_MAX_T) down_flag = 1;
//    if(pitch_tar_angel <= PITCH_MIN_T) down_flag = 0;
//	
//    if(down_flag) pitch_tar_angel -=PITCH_DELTA;
//    else pitch_tar_angel +=PITCH_DELTA;
//	
//}
//float myabs(float input){
//	return input>0 ? input : -input;
//}
///*********************** AutoAim ************************/
//MSGrecFromUpper_TypeDef upper_cmd;
//uint32_t pack_ID_save = 0;
//float auto_yaw_tar = 0;
//float auto_pit_tar = 0;
//void cStateAction::autoAimStateAction(void){
//    // if(S2A_flag){
//    //     S2AchangePIDParam();
//    //     S2A_flag = false;
//    //     A2S_flag = true;
//	// 	toDBUS_flag = true;
//	// 	auto_yaw_tar = getMotorMsgFromCan(GMYAW_e)->angelAll;
//	// 	auto_pit_tar = getMotorMsgFromCan(GMPitch_e)->angelAll;
//    // }
//    if(last_state_ == scout_e || last_state_ == dbus_e ){
//        S2AchangePIDParam();
//        auto_yaw_tar = getMotorMsgFromCan(GMYAW_e)->angelAll;
//	  	auto_pit_tar = getMotorMsgFromCan(GMPitch_e)->angel;
//         last_state_ = now_state_; 
//    }
//	GMCtrlSetTarget(GMYAW_e,auto_yaw_tar);
//	GMCtrlSetTarget(GMPitch_e,auto_pit_tar);
//	upper_cmd= *getUpperCtrlInfo();

//	if(upper_cmd.packetID >  pack_ID_save ){
//		#if Gimbal_TOP_DEFINE  //TBSYCY1
//		auto_pit_tar = getMotorMsgFromCan(GMPitch_e)->angel + 1.1*upper_cmd.pitchTarAngel;
//		auto_yaw_tar  = getMotorMsgFromCan(GMYAW_e)->angelAll + 1.1*upper_cmd.yawTarAngel;	
//		
//		#elif  Gimbal_BOTTOM_DEFINE 
//		auto_pit_tar = getMotorMsgFromCan(GMPitch_e)->angelAll - 1.0*upper_cmd.pitchTarAngel;
//		auto_yaw_tar  = getMotorMsgFromCan(GMYAW_e)->angelAll + 1.0*upper_cmd.yawTarAngel;	
//		
//		#endif
//		
//		pack_ID_save = upper_cmd.packetID ;
//	}
//	if(myabs( upper_cmd.packetID - pack_ID_save)>2000 )
//		pack_ID_save = upper_cmd.packetID ;

//    if(fire_speed_ == fireSpeed_stop_e){
//        GMCtrlSetTarget(GMFriction0_e , 0);
//        GMCtrlSetTarget(GMFriction1_e , 0);
//        GMCtrlSetTarget(GMRammer_e , 0);
//    }
//    else if(fire_speed_ == fireSpeed_low_e){
//        switch(fire_freq_){
//            case fireFreq_stop_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR * FRIC_SPEED_LOW);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR * FRIC_SPEED_LOW);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , 0);
//                break;
//            }
//            case fireFreq_low_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR * FRIC_SPEED_LOW);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR * FRIC_SPEED_LOW);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_LOW);
//                break;
//            }
//            case fireFreq_mid_e : {
//                GMCtrlSetTarget(GMFriction0_e ,  FRI0_DIR * FRIC_SPEED_LOW);
//                GMCtrlSetTarget(GMFriction1_e ,  FRI1_DIR * FRIC_SPEED_LOW);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_MID);
//                break;
//            }
//            case fireFreq_high_e : {
//                GMCtrlSetTarget(GMFriction0_e ,  FRI0_DIR * FRIC_SPEED_LOW);
//                GMCtrlSetTarget(GMFriction1_e ,  FRI1_DIR * FRIC_SPEED_LOW);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_HIGH);
//                break;
//            }
//        }
//    }
//    else if(fire_speed_ == fireSpeed_mid_e){
//        switch(fire_freq_){
//            case fireFreq_stop_e : {
//                GMCtrlSetTarget(GMFriction0_e ,  FRI0_DIR *FRIC_SPEED_MID);
//                GMCtrlSetTarget(GMFriction1_e ,  FRI1_DIR *FRIC_SPEED_MID);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , 0);
//                break;
//            }
//            case fireFreq_low_e : {
//                GMCtrlSetTarget(GMFriction0_e ,  FRI0_DIR *	FRIC_SPEED_MID);
//                GMCtrlSetTarget(GMFriction1_e ,  FRI1_DIR *	FRIC_SPEED_MID);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_LOW);
//                break;
//            }
//            case fireFreq_mid_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR * FRIC_SPEED_MID);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR * FRIC_SPEED_MID);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_MID);
//                break;

//            }
//            case fireFreq_high_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR * FRIC_SPEED_MID);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR * FRIC_SPEED_MID);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_HIGH);
//                break;
//            }
//        }

//    }
//    else if(fire_speed_ == fireSpeed_high_e){
//        switch(fire_freq_){
//            case fireFreq_stop_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR * FRIC_SPEED_HIGH);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR *  FRIC_SPEED_HIGH);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , 0);
//                break;
//            }
//            case fireFreq_low_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR *  FRIC_SPEED_HIGH);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR * FRIC_SPEED_HIGH);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_LOW);
//                break;
//            }
//            case fireFreq_mid_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR * FRIC_SPEED_HIGH);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR * FRIC_SPEED_HIGH);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e , RAMMER_POS_DIR * RAMMER_SPEED_MID);
//                break;

//            }
//            case fireFreq_high_e : {
//                GMCtrlSetTarget(GMFriction0_e , FRI0_DIR * FRIC_SPEED_HIGH);
//                GMCtrlSetTarget(GMFriction1_e , FRI1_DIR * FRIC_SPEED_HIGH);
//                if(isCouldShoot())
//                    GMCtrlSetTarget(GMRammer_e ,RAMMER_POS_DIR * RAMMER_SPEED_HIGH);
//                break;
//            }
//        }
//    }
//    else return;
//}

///*********************** Dbus ************************/

//RC_CtrlTypeDef  dbus_msg;
//bool reset_flag = true;
//bool block_flag  = false;
//void cStateAction::dbusStateAction(void){
//    // if(toDBUS_flag){
//    //     S2AchangePIDParam();
//    //     S2A_flag = true;
//    //     A2S_flag = true;
//	// 	toDBUS_flag = false;	
//	// 	yaw_target_angel = getMotorMsgFromCan(GMYAW_e)->angelAll;
//	// 	pitch_target_angel = getMotorMsgFromCan(GMPitch_e)->angel;
//    // }
//    if(last_state_ == scout_e || last_state_ == autoAim_e || last_state_ == prepare_e){
//        S2AchangePIDParam();
//        yaw_target_angel = getMotorMsgFromCan(GMYAW_e)->angelAll;
//	 	pitch_target_angel = getMotorMsgFromCan(GMPitch_e)->angel;
//         last_state_ = now_state_;
//    }
//	if(reset_flag){
//		yaw_target_angel = getMotorMsgFromCan(GMYAW_e)->angelAll;
//	 	pitch_target_angel = getMotorMsgFromCan(GMPitch_e)->angel;
//		reset_flag = false;
//	}
//	GMCtrlSetTarget(GMYAW_e,yaw_target_angel);
//    GMCtrlSetTarget(GMPitch_e,pitch_target_angel);

//    #if Gimbal_TOP_DEFINE
//    dbus_msg= RC_GetDatas();
//    if(dbus_msg.rc.s1 == 3 || dbus_msg.rc.s1 == 2) {
//		now_state_ = scout_e;
//        scoutStateAction();
//    }
//    else if(dbus_msg.rc.s1 == 1){ //遥控器控制上云台和底盘左右移动
//        //获取YAW的角度
//	    if(dbus_msg.rc.ch2==1024) yaw_target_angel=yaw_target_angel;
//	    else yaw_target_angel += (float)(dbus_msg.rc.ch2-1024)*0.001215;	//Yaw
//	
//	    //pitch限幅 Date 6.21 好像没必要
//	    //if(pitch_target_angel>=PITCH_ANGEL_MAX) pitch_target_angel = PITCH_ANGEL_MAX;
//	    //else if (pitch_target_angel<=PITCH_ANGEL_MIN) pitch_target_angel = PITCH_ANGEL_MIN;
//	
//	    #if Gimbal_TOP_DEFINE  //TBSYCY
//		    pitch_target_angel += (float)(dbus_msg.rc.ch3 - 1024)*0.0005515;
//	    #elif Gimbal_BOTTOM_DEFINE
//		    pitch_target_angel += (float)(1024-dbus_msg.rc.ch3)*0.0007515;
//	    #endif

//        //摩擦轮测速
//        if(dbus_msg.rc.s2 == 2){
//            GMCtrlSetTarget(GMFriction0_e,0);
//            GMCtrlSetTarget(GMFriction1_e,0);
//            GMCtrlSetTarget(GMRammer_e,0);
//        }
//        else if (dbus_msg.rc.s2 == 3){
//		    GMCtrlSetTarget(GMFriction0_e,FRI0_DIR*FRIC_SPEED_MID);
//            GMCtrlSetTarget(GMFriction1_e,FRI1_DIR*FRIC_SPEED_MID);
//            if(dbus_msg.rc.ch1 > 1500){
//                GMCtrlSetTarget(GMRammer_e,RAMMER_POS_DIR * RAMMER_SPEED_MID);
//            }
//            else if (dbus_msg.rc.ch1 < 500){
//                GMCtrlSetTarget(GMRammer_e,RAMMER_NEG_DIR*RAMMER_SPEED_INV);
//            } 
//			else 
//				GMCtrlSetTarget(GMRammer_e,0);
//        } 
//        else if(dbus_msg.rc.s2 == 1){
//            GMCtrlSetTarget(GMFriction0_e,FRI0_DIR*FRIC_SPEED_HIGH);
//            GMCtrlSetTarget(GMFriction1_e,FRI1_DIR*FRIC_SPEED_HIGH);
//            if(dbus_msg.rc.ch1 > 1300){ 
//				//卡弹处理
//					lockedRotorPrc(block_flag);
//					//end
//					//热量控制 begin
//					if(!block_flag)					
//						GMCtrlSetTarget(GMRammer_e,5000*RAMMER_POS_DIR);
//						//shootHeatCtrl(getJudgeMsgFromCan()->shootHeat_Lv);
//			}
//			else if (dbus_msg.rc.ch1 < 500){
//                GMCtrlSetTarget(GMRammer_e,RAMMER_NEG_DIR*RAMMER_SPEED_INV);
//            } 
//			else {
//				GMCtrlSetTarget(GMRammer_e,0);
//			}
//                //
//            }
//					
//        }
//     
//	else return;

//    #elif Gimbal_BOTTOM_DEFINE
//    sDbusMsgFromTop  dbus_msg= getDbusMsgFromCanCom();  
//    if(dbus_msg.s1 == 3 || dbus_msg.s1 == 1) {
//		now_state_ = scout_e;
//        scoutStateAction();
//    }
//    else if(dbus_msg.s1 == 2){ //遥控器控制上云台和底盘左右移动
//        //获取YAW的角
//	    if(dbus_msg.ch2==1024) yaw_target_angel=yaw_target_angel;
//	    else yaw_target_angel += (float)(dbus_msg.ch2-1024)*0.001215;	//Yaw
//	
//	    #if Gimbal_TOP_DEFINE  //TBSYCY
//		    pitch_target_angel += (float)(dbus_msg.rc.ch3 - 1024)*0.0007515;
//	    #elif Gimbal_BOTTOM_DEFINE
//		    pitch_target_angel += (float)(dbus_msg.ch3-1024)*0.001015;
//			GMCtrlSetTarget(GMPitch_e,pitch_target_angel);
//	    #endif

//        //摩擦轮测速
//        if(dbus_msg.s2 == 2){
//            GMCtrlSetTarget(GMFriction0_e,0);
//            GMCtrlSetTarget(GMFriction1_e,0);
//            GMCtrlSetTarget(GMRammer_e,0);
//        }
//        else if (dbus_msg.s2 == 3){
//		    GMCtrlSetTarget(GMFriction0_e,FRI0_DIR*FRIC_SPEED_MID);
//            GMCtrlSetTarget(GMFriction1_e,FRI1_DIR*FRIC_SPEED_MID);
//            if(dbus_msg.ch1 > 1500){
//                GMCtrlSetTarget(GMRammer_e,RAMMER_POS_DIR * RAMMER_SPEED_MID);
//            }
//            else if (dbus_msg.ch1 < 500){
//                GMCtrlSetTarget(GMRammer_e,RAMMER_NEG_DIR*RAMMER_SPEED_INV);
//            }
//			else 
//				GMCtrlSetTarget(GMRammer_e,0);
//			
//        } 
//        else if(dbus_msg.s2 == 1){
//            GMCtrlSetTarget(GMFriction0_e,FRI0_DIR*FRIC_SPEED_HIGH);
//            GMCtrlSetTarget(GMFriction1_e,FRI1_DIR*FRIC_SPEED_HIGH);
//            if(dbus_msg.ch1 > 1300){ 
//				//卡弹处理
//					lockedRotorPrc(block_flag);
//					//end
//					//热量控制 begin
//					if(!block_flag)					
//						GMCtrlSetTarget(GMRammer_e,5000*RAMMER_POS_DIR);
//						//shootHeatCtrl(getJudgeMsgFromCan()->shootHeat_Lv);
//			}
//			else if (dbus_msg.ch1 < 500){
//                GMCtrlSetTarget(GMRammer_e,RAMMER_NEG_DIR*RAMMER_SPEED_INV);
//            }
//			else 
//				GMCtrlSetTarget(GMRammer_e,0);
//        }
//    }  
//    #endif










////////////////////// old dbus control begin///////////////////
////    ////////////////////cutting line///////////////////// 
////	//获取YAW的角度
////	if(RC_GetDatas().rc.ch2==1024) yaw_target_angel=yaw_target_angel;
////	else yaw_target_angel += (float)(RC_GetDatas().rc.ch2-1024)*0.001215;	//Yaw
////	
////	//获取pitch的角度
////	if(pitch_target_angel>=PITCH_ANGEL_MAX) pitch_target_angel = PITCH_ANGEL_MAX;
////	else if (pitch_target_angel<=PITCH_ANGEL_MIN) pitch_target_angel = PITCH_ANGEL_MIN;
////	
////	#if Gimbal_TOP_DEFINE  //TBSYCY
////		pitch_target_angel += (float)(RC_GetDatas().rc.ch3 - 1024)*0.0007515;
////	#elif Gimbal_BOTTOM_DEFINE
////		pitch_target_angel += (float)(1024-RC_GetDatas().rc.ch3)*0.0007515;
////	#endif
////	
////    //摩擦轮测速
////    if(RC_GetDatas().rc.s1 == 3){
////        GMCtrlSetTarget(GMFriction0_e,0);
////        GMCtrlSetTarget(GMFriction1_e,0);
////        GMCtrlSetTarget(GMRammer_e,0);
////    }
////    else if (RC_GetDatas().rc.s1 == 1){

////		GMCtrlSetTarget(GMFriction0_e,FRI0_DIR*FRIC_SPEED_HIGH);
////        GMCtrlSetTarget(GMFriction1_e,FRI1_DIR*FRIC_SPEED_HIGH);
////		
////        if(RC_GetDatas().rc.s2 == 1){
////            GMCtrlSetTarget(GMRammer_e,RAMMER_POS_DIR*2000);
////        }
////        else if (RC_GetDatas().rc.s2 == 2){
////            if(RC_GetDatas().rc.ch1 > 1500){
////                GMCtrlSetTarget(GMRammer_e,RAMMER_POS_DIR*6500);
////            }
////            else if (RC_GetDatas().rc.ch1 < 500){
////                GMCtrlSetTarget(GMRammer_e,RAMMER_NEG_DIR*800);
////            }  
////			else 
////				GMCtrlSetTarget(GMRammer_e,0);
////        } 
////    }
////////////////////// old dbus control end///////////////////
//    
//}
////ycy！！！发现暂时无用
//void cStateAction::stateSwitch(void){
//    if(upper_cmd_ == TO_AUTOAIM_MODE_e){
//        now_state_ = autoAim_e;
//    }
//    else if(upper_cmd_ == TO_SCOUT_MODE_e){
//        now_state_ = scout_e;
//    }
//    else return ;
//}

//bool cStateAction::isCouldShoot(void){
//    if(getUpperCtrlInfo()->isShoot == 0x4b)
//		return true;
//	else return false;
//}

//void cStateAction::lockedRotorPrc(bool& block_flag){
////	printf("A:%d,%d,%d,%d\n",
////			getMotorMsgFromCan(GMRammer_e)->torque 
////			,abs( getMotorMsgFromCan(GMRammer_e)->speed) <100 
////			,reverse_time_ == 0 
////			,block_flag == false);
//	
//	if(getMotorMsgFromCan(GMRammer_e)->torque < 5000 * RAMMER_POS_DIR
//			&& abs( getMotorMsgFromCan(GMRammer_e)->speed) <100 
//			&& reverse_time_ == 0 && block_flag == false)
//	{
//			block_flag = true;
//			reverse_time_ = 500;
//	}
//	if(reverse_time_){
//		reverse_time_ -- ;
//		GMCtrlSetTarget(GMRammer_e,2000*RAMMER_NEG_DIR);
//	}
//	else{
//		block_flag = false;
//	}
//}

//#define LV1 0
//#define LV2 100
//#define LV3 200
//#define LV4 250
//#define LV5 300
//void cStateAction::shootHeatCtrl(uint16_t heat){
//	if(heat>=LV1 && heat<=LV2)
//		setFireFreq(fireFreq_high_e);
//	else if(heat>LV2 && heat <=LV3)
//		setFireFreq(fireFreq_mid_e);
//	else if(heat>LV3 && heat <=LV4)
//		setFireFreq(fireFreq_low_e);
//	else if(heat>LV4 && heat <=LV5)
//		setFireFreq(fireFreq_stop_e);
//	else 
//		setFireFreq(fireFreq_mid_e);
//		
//}

















