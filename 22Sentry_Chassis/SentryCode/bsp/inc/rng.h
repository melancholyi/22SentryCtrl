#ifndef _BSP_RNG_H_
#define _BSP_RNG_H_
 
#include "sys.h" 


uint8_t  RNG_Init(void);			//RNG初始化 
//uint32_t RNG_Get_RandomNum(void);//得到随机数
int16_t getRandomNum(int16_t min,int16_t max);
int16_t getPosOrNegNum(void);

#endif // !_BSP_RNG_H_