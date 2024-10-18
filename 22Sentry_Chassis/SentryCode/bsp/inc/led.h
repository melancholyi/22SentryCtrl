#ifndef _LED_H_
#define _LED_H_
#include "sys.h"



#define LED_ON  1
#define LED_OFF 0
#define LED_Red   PCout(1)
#define LED_Green PCout(0)


class LED{

public:
	void LED_Init(void);	
private:
	void LED_Configuration(void);	
};



#endif