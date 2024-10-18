#include "FreeRTOS.h" //FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
#include "led.h"
#include "iwdg.h"
#include "VisionCom.h"
#include "beep.h"

#include "pid.h"

void LED::LED_Configuration(void){
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOH, &GPIO_InitStructure);

}

void LED::LED_Init(void){         
	LED_Configuration();
    LED_Red=LED_OFF;
    LED_Green=LED_OFF;
	LED_Blue=LED_OFF;
}


#define LED_TASK_PERIOD 5000
void Led_Task(void *pvParameters)
{
	LED led;
	/*LED初始化*/
	led.LED_Init();
	LED_Red = !LED_Red;
	portTickType currentTime2;
	while (1)
	{
		currentTime2 = xTaskGetTickCount();	
		LED_Red = !LED_Red;
		LED_Green = !LED_Green;
		LED_Blue = !LED_Blue;
	
		vTaskDelayUntil(&currentTime2, LED_TASK_PERIOD);
	}
}

