#include "rng.h"

//��ʼ��RNG
uint8_t RNG_Init(void)
{
	u16 retry=0; 
	
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);//����RNGʱ��,����PLL48CLK
	
	RNG_Cmd(ENABLE);	//ʹ��RNG
	
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET&&retry<10000)	//�ȴ����������
	{
		retry++;
		delayUs(100);
	}
	if(retry>=10000)return 1;//���������������������
	return 0;
}
//�õ������
//����ֵ:��ȡ���������
uint32_t RNG_Get_RandomNum(void)
{	 
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);	//�ȴ����������  
	return RNG_GetRandomNumber();	
}

//����[min,max]��Χ�������
int16_t getRandomNum(int16_t min,int16_t max)
{ 
   return RNG_Get_RandomNum()%(max-min+1) +min;
}

int16_t getPosOrNegNum(void){
	int16_t temp = getRandomNum(2,5000);
	return ((temp>2500) ? -1:1);
}




