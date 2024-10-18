/**MADE BY ME
  **************************************************************************** 	
  * @author  LIU_Standard
  * @version V1.0.0
  * @date    15/9/2021
  * @brief   ��ң�ؽ������ڣ��ο���һ�첽������͹ٷ�ʹ���ֲ�
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

/** @defgroup RC Ŀǰֻ������ң��������
 *  @brief    ���������õĽṹ��
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
	}rc;				 	//ң����
	__PACKED struct
	{
		int16_t x;
		int16_t y;
		int16_t z;
		uint8_t press_l;
		uint8_t press_r;
	}mouse;					//���
	__PACKED struct
	{
		uint16_t v;			//����
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
