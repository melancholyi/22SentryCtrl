#ifndef _LED_H_
#define _LED_H_

//#ifdef __cplusplus
//extern "C" {
//#endif
	
#include "sys.h"

#define LED_ON  1
#define LED_OFF 0
#define LED_Red   PHout(12)
#define LED_Green PHout(11)
#define LED_Blue  PHout(10)

class LED{

public:
	void LED_Init(void);	
private:
	void LED_Configuration(void);	
};

//#ifdef __cplusplus
//}
//#endif

#endif