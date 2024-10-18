//#ifndef __STATA_ACTION__H
//#define __STATA_ACTION__H

//#include "control.h"
//#include "gm2ch.h"

//enum eState{
//    prepare_e = 0,
//    scout_e  = 1,
//    autoAim_e = 2 ,
//	dbus_e = 3
//	
//};

//enum eFireLv{ //control Frico speed  ---> fire speed
//    fireSpeed_stop_e = 0,
//    fireSpeed_low_e = 1,
//    fireSpeed_mid_e = 2,
//    fireSpeed_high_e = 3
//};
//enum eFireFreq{ //control rammer speed ---> fire frequence 
//    fireFreq_stop_e = 0,
//    fireFreq_low_e = 1,
//    fireFreq_mid_e = 2,
//    fireFreq_high_e = 3
//};

//enum eUpperCMD{
//    NULL_SEND_MODE_e = 0x01,
//    TO_SCOUT_MODE_e  =  0x2a,
//    TO_AUTOAIM_MODE_e = 0x3a
//};


//class cStateAction{
//    private:
//        eState      now_state_; 
//		eState      last_state_;
//        eUpperCMD   upper_cmd_;
//        eFireLv     fire_speed_;
//        uint16_t    cool_time_;
//        bool        is_over_heat_;
//        eFireFreq   fire_freq_;
//        uint16_t locked_rotor_;        
//        uint16_t reverse_time_;
//        //funcution
//        void prepareStateAction(void);
//        void scoutStateAction(void);
//        void autoAimStateAction(void);
//        void dbusStateAction(void); 
//        void stateSwitch(void);
//        

//    public:
//        cStateAction();//默认构造函数 准备状态 
//        eState getNowState(void) const;
//        void lockedRotorPrc(bool& block_flag);

//        void setState(eState state);
//        void setFireLv(eFireLv lv);
//        void setFireFreq(eFireFreq freq); 
//        bool isCouldShoot(void);
//        void stateActionCtrlLoop(void);
//		void shootHeatCtrl(uint16_t heat);
//};



//#endif /*__STATA_ACTION__H*/

