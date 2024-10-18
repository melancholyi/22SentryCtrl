/*****************************************************************************
File name: TDT_Task\src\board.h
Description: �δ�ʱ���ĳ�ʼ������ʱ���ܣ���ʼ��

Author: �洫
Version: 1.1.1.191112_alpha
Date: 19.11.12
History: 
	����������������������������������������������������������������������������������������������������������������������������������������������������
	19.11.12 �״μ�¼
	����������������������������������������������������������������������������������������������������������������������������������������������������
*****************************************************************************/
#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/***************Ӳ���жϷ���******************/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"
#include "gpio.h"
//DSP��
#include "arm_math.h"
//#include "TDT_USER.h"
	 


#define USE_JUDGEMENT 0



//�δ�ʱ���ٷ�Ƶ��MPRE���ж�Ϊ1ms
#define MPRE 4
//#define OS

#ifndef _Bool
	#define _Bool u8
#endif
enum IsEnable{
	enable=1,disable=0
};

//���º궨�����ڻ�ȡö�ٸ�����ͨ��������,��Ҫ�ֱ������������꣬��'|'�ڵ�����ΪENUM_NUM(x)�е�x������ö���в��ܺ��лس����ɷ���keil�Ĵ���ģ��Templates��
#define ENUM_START(x) const unsigned char x##_ENUM_START_LINE = __LINE__;enum x
#define ENUM_END(x) const unsigned char x##_ENUM_END_LINE = __LINE__;
//���º��ȡ��ݸ���ʱʹ�ã���Ҫ��ö�ٶ��崦��ͬһ�������ռ���
#define ENUM_NUM(x) (x##_ENUM_END_LINE - x##_ENUM_START_LINE - 3)
//���º궨��ö�ٱ���ʱʹ��
#define ENUM(x) x
//���º궨����������ö�ٵ�++�����������������һ��.c(.cpp)��ʹ��
#define USE_OPERATOR_AUTO_INC(x) inline x operator++(x &rs,int){x oldEnum = rs;if ((int)rs < ENUM_NUM(x) - 1)rs = (x)(rs + 1);else rs = (x)0;return oldEnum;}


extern volatile uint32_t sysTickUptime;
extern volatile uint32_t sysTickUptimeUs;

void sysTickInit(void);
uint64_t getSysTimeUs(void);
void delayUs(uint32_t us);
void delayMs(uint32_t ms);
void boardALLInit(void);
float timeIntervalFrom_f(float timeFrom);
uint32_t timeIntervalFrom(uint32_t timeFrom);

#ifdef __cplusplus
	extern "C" {
#endif
	
void delayUsTask(u32 us);
	
#ifdef __cplusplus
}
#endif


#endif /* __BOARD_H__ */

