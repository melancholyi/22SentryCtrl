#ifndef _CAN_H_
#define _CAN_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f4xx.h"

void CAN1_Init(void);
void CAN2_Init(void);
int  sendToClassis_test(int16_t sendChassisPos);
#ifdef __cplusplus
}
#endif

#endif 
