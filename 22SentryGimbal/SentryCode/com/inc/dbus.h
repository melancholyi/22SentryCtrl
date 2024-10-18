/**MADE BY ME
  **************************************************************************** 	
  * @author  LIU_Standard
  * @version V1.0.0
  * @date    15/9/2021
  * @brief   大疆遥控接收所在，参考上一届步兵代码和官方使用手册
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _DEBUS_H_
#define _DEBUS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"



/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stddef.h"
/* defines -------------------------------------------------------------------*/
#define RC_CH_VALUE_MIN 	((uint16_t)364 )
#define RC_CH_VALUE_OFFSET 	((uint16_t)1024)
#define RC_CH_VALUE_MAX 	((uint16_t)1684)

/* Exported types ------------------------------------------------------------*/

/** @defgroup RC 目前只测试了遥控器操作
 *  @brief    接收数据用的结构体
 *
 */

typedef  struct 
{
	__PACKED struct 
	{
		int16_t ch0;
		int16_t ch1;
		int16_t ch2;
		int16_t ch3;
		uint8_t s1;
		uint8_t s2;
	}rc;				 	//遥控器
	__PACKED struct
	{
		int16_t x;
		int16_t y;
		int16_t z;
		uint8_t press_l;
		uint8_t press_r;
	}mouse;					//鼠标
	__PACKED struct
	{
		uint16_t v;			//键盘
	}key;
}__attribute__((packed)) RC_CtrlTypeDef;  //USE ACV6 and C++

/* Functions -----------------------------------------------------------------*/
RC_CtrlTypeDef* getRCInfo(void);
void RC_DataProcess(uint8_t *pData);
void RC_Dbus_Init(void);

/*************MADE BY LIU****************END OF FILE***************/

#ifdef __cplusplus
}
#endif
#endif
