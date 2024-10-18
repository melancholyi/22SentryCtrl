/******************************
File name: TDT_Bsp\src\timer.cpp
Description: 定时�?-主�?�也为freertos提供了一�?时基
function:
	—————————————————————————————————————————————————————————————————————————�?
	void ConfigureTimeForRunTimeStats(void)
	—————————————————————————————————————————————————————————————————————————�?
	static void TIM2_Init(u16 arr,u16 psc)
	—————————————————————————————————————————————————————————————————————————�?
	void TIM2_IRQHandler(void)
	—————————————————————————————————————————————————————————————————————————�?
Author: 肖银�?
Version: 1.1.1.191112_alpha
Date: 19.11.12
History: 
	—————————————————————————————————————————————————————————————————————————�?
	19.11.12 首�?�完�?
	—————————————————————————————————————————————————————————————————————————�?
****************************  */
#include "timer.h"
#include "sys.h"


//FreeRTOS时间统�?�所用的节拍计数�?
volatile unsigned long long FreeRTOSRunTimeTicks;
static void TIM2_Init(u16 arr,u16 psc);


/**
  * @brief 初�?�化TIM2使其为FreeRTOS的时间统计提供时�?
  */
void ConfigureTimeForRunTimeStats(void)
{
	//定时�?3初�?�化，定时器时钟�?84M，分频系数为84-1，所以定时器3的�?�率
	//�?84M/84=1M，自动重装载�?50-1，那么定时器周期就是50us
	FreeRTOSRunTimeTicks=0;
	TIM2_Init(50-1,84-1);	//初�?�化TIM2
}



/**
  * @brief 定时�?2�?�?初�?�化
  * @param [�?动重装值，时钟预分频数]
  * @note 时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
			Ft=定时器工作�?�率,单位:Mhz
  */
static void TIM2_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM2时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�?动重装载�?
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分�?
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初�?�化TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允�?�定时器3更新�?�?
	TIM_Cmd(TIM2,ENABLE); //使能定时�?3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时�?3�?�?
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先�?1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



/**
  * @brief 定时�?2�?�?服务函数
  */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出�?�?
	{
		FreeRTOSRunTimeTicks++;
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除�?�?标志�?
}


