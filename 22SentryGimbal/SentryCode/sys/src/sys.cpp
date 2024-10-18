/******************************
File name: TDT_Task\src\board.cpp
Description: �δ�ʱ���ĳ�ʼ������ʱ���ܣ���ʼ��
function:
	����������������������������������������������������������������������������������������������������������������������������������������������������
	void TDT_SysTick_Init(void)
	����������������������������������������������������������������������������������������������������������������������������������������������������
	uint32_t GetSysTime_us(void)
	����������������������������������������������������������������������������������������������������������������������������������������������������
	void DelayUs(uint32_t us)
	����������������������������������������������������������������������������������������������������������������������������������������������������
	void DelayMs(uint32_t ms)
	����������������������������������������������������������������������������������������������������������������������������������������������������
	void DelayUs_Task(u32 nms)
	����������������������������������������������������������������������������������������������������������������������������������������������������
	void TDT_Board_ALL_Init(void)
	����������������������������������������������������������������������������������������������������������������������������������������������������
Author: �洫
Version: 1.1.1.191112_alpha
Date: 19.11.12
History:
	����������������������������������������������������������������������������������������������������������������������������������������������������
	19.11.12 �״μ�¼
	����������������������������������������������������������������������������������������������������������������������������������������������������
****************************  */
#include "sys.h"


/**FreeRTOS*START***************/
#include "FreeRTOS.h"					//FreeRTOSʹ��
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/
#include "cycle.h"
#include "iwdg.h"
/***�궨��***/
#define TICK_PER_SECOND 1000
#define TICK_US	(1000000/TICK_PER_SECOND)

/***ȫ�ֱ���***/
/*��ʼ����ɱ�־*/
u8 Init_OK;
//�δ�ʱ���������� ,49������
volatile uint32_t sysTickUptime=0;
volatile uint32_t sysTickUptimeUs=0;
int* * *a;
/**
  * @brief ��ʼ���δ�ʱ��
  * @note ����޸��ⲿ����,�ǵ��޸� HSE_VALUE��PLL_M
  */
void sysTickInit(void)
{

	RCC_ClocksTypeDef  rcc_clocks;
	uint32_t           cnts;

	RCC_GetClocksFreq(&rcc_clocks);

	cnts = (uint32_t)rcc_clocks.HCLK_Frequency / TICK_PER_SECOND;
	cnts = cnts / (8*MPRE);//�δ�ʱ��1/4ms����һ���ж�
	//cnts=168000/8;=1ms
	SysTick_Config(cnts);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}




/**
  * @brief ��ȡus��ʱ��
  * @return us��ʱ��
  * @note �δ�ʱ����Ƶ�ڴ˺�������������ʧ����
  */
uint64_t getSysTimeUs(void)
{
	uint64_t ms;
	uint64_t value;
	ms = sysTickUptime;
	value = (ms * TICK_US + (SysTick->LOAD - SysTick->VAL) * TICK_US / SysTick->LOAD) / MPRE;
	return value;
}

float timeIntervalFrom_f(float timeFrom)
{
	return float(getSysTimeUs()) / 1e6f - timeFrom;
}

uint32_t timeIntervalFrom(uint32_t timeFrom)
{
	return getSysTimeUs() - timeFrom;
}

/**
  * @brief US����ʱ
  * @param us ��ʱʱ������λUS
  * @note ע���ڲ���ϵͳ������ʹ�ô˺�����ؽ����ٽ��������򾫶Ȳ���֤
		����ʹ�õδ���ʱ����ɼ���Ŀ�����ʱ��Խ��������Խ�󣬾�����С���ٺ�������
  */
void delayUs(uint32_t us)
{
	uint32_t now = getSysTimeUs();
	uint32_t cnt = 0;
	uint32_t ticks = 0;
	uint32_t told = 0;
	uint32_t tnow = 0;
	uint32_t tcnt = 0;
	uint32_t reload = 0;
	reload = SysTick->LOAD;
	ticks = us * 168 / 8;
	told = SysTick->VAL;
	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			if (tnow < told)
			{
				tcnt += told - tnow;
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			if (tcnt >= ticks)
			{
				break;
			}
		}
		//iwdgFeed();
	}
	cnt = getSysTimeUs() - now;
}



/**
  * @brief MS����ʱ
  * @param ms ��ʱʱ������λMS
  * @return
  * @note ע���ڲ���ϵͳ������ʹ�ô˺�����ؽ����ٽ��������򾫶Ȳ���֤
  */
void delayMs(uint32_t ms)
{
	delayUs(ms*1000);
}



/**
  * @brief ������ʱ����
  * @param us��ʱʱ������λUS
  * @note Ҫ��ʱ��us��
  */
void delayUsTask(u32 us)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
	{
		if(us>=1000)						//��ʱ��ʱ�����OS������ʱ������
		{
   			vTaskDelay(pdMS_TO_TICKS(us/1000));	 		//FreeRTOS��ʱ
		}
		us%=1000;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ
	}
	delayUs((u32)(us));				//��ͨ��ʽ��ʱ
}



/**
  * @brief �ܳ�ʼ������
  * @note ��ֹʹ����ʱ
  */
void boardALLInit(void)
{
	/* ��ֹȫ���ж�*/
	__disable_irq();

//	Vision_Init();
	 /*���Ź���ʼ��-ι����LED*/
	//iwdgInit(4,50);
	//can1_init
	//can2����init
	/*��ʼ�����*/
	Init_OK = 1;
	/*  ʹ��ȫ���ж� */
	__enable_irq();
}
/***********End of file*****************  */
