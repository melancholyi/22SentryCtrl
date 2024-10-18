/**				
*@brief				CAN1��ʼ�������ڿ���ֱ����ˢ���GM6020�������жϺͷ��͵�������غ�����M6020Control.c��
*@author			choly
*@time				2021//
*@attention		
**/

#include "can.h"
#include "sys.h"
/**********************************************************************
*���ܣ�Can1�ĳ�ʼ������,ͨ�š��͵��ͨ�Ż�ȡ���״̬
***********************************************************************/

void CAN1_Init(void)
{
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    GPIO_InitTypeDef       GPIO_InitStructure;
    NVIC_InitTypeDef       NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);//���ù���
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);
    
    CAN_InitStructure.CAN_TTCM = DISABLE;//��ʱ�䴥��ͨ��ģʽ
    CAN_InitStructure.CAN_ABOM = DISABLE;//����Զ����߹���
    CAN_InitStructure.CAN_AWUM = DISABLE;//˯��ģʽͨ��������ѣ����CAN->MCR��SLEEPλ)
    CAN_InitStructure.CAN_NART = DISABLE;//CAN Ӳ�����Զ��ط�����Ϣ��ֱ������ CAN ��׼��Ϣ���ͳɹ�
    CAN_InitStructure.CAN_RFLM = DISABLE;//FIFO���Ĳ������������µĸ��Ǿɵ�
    CAN_InitStructure.CAN_TXFP = ENABLE;//���ȼ�������˳��ʱ��˳��ȷ��
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
    CAN_InitStructure.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN1, &CAN_InitStructure);

    CAN_FilterInitStructure.CAN_FilterNumber = 0;//������0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//32λ
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;//32λID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//32λMASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;//������0������FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;//���������0
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0�յ���Ϣ�ж�����		   
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);//��������Ϊ���ж� 
}



void CAN2_Init(void){
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2); 
    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5 ;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOB, &gpio);
    nvic.NVIC_IRQChannel = CAN2_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
//    nvic.NVIC_IRQChannel = CAN2_TX_IRQn;
//    nvic.NVIC_IRQChannelPreemptionPriority = 1;
//    nvic.NVIC_IRQChannelSubPriority = 1;
//    nvic.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&nvic);
    CAN_DeInit(CAN2);
    CAN_StructInit(&can);
    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;    
    can.CAN_AWUM = DISABLE;    
    can.CAN_NART = DISABLE;    
    can.CAN_RFLM = DISABLE;    
    can.CAN_TXFP = ENABLE;     
    can.CAN_Mode = CAN_Mode_Normal; 
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
    can.CAN_Prescaler = 3;
    CAN_Init(CAN2, &can);
    can_filter.CAN_FilterNumber=14;
    can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filter.CAN_FilterIdHigh=0x0000;
    can_filter.CAN_FilterIdLow=0x0000;
    can_filter.CAN_FilterMaskIdHigh=0x0000;
    can_filter.CAN_FilterMaskIdLow=0x0000;
    can_filter.CAN_FilterFIFOAssignment=0;
    can_filter.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filter);
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);
    CAN_ITConfig(CAN2,CAN_IT_TME,ENABLE);
}


//int  sendToClassis_test(int16_t sendChassisPos)
//{
//	//data0:	����λ��ʾ������ң�������ƻ���NX���ƣ���Ϊң�������ƣ�ʣ��λΪ�����ٶȸ�4λ
//	//data1:  ��Ϊң�������ƣ�Ϊ�����ٶȵͰ�λ������Ϊָ��λ����Ϣ��0-255��
//	CanTxMsg TxMessage;
//	TxMessage.StdId = 0x392;
//	TxMessage.RTR = CAN_RTR_DATA;
//	TxMessage.IDE = CAN_Id_Standard;
//	TxMessage.DLC = 0x08;
//	TxMessage.Data[0] = ((uint8_t)(sendChassisPos>> 8));						//����λΪ00
//	TxMessage.Data[1]=(uint8_t)sendChassisPos;	
//	//debug ���ͽ�����־
//	//printf("sendMsg:ID: %d DATA: %d %d \r\n",TxMessage.StdId,TxMessage.Data[0],TxMessage.Data[1]);
//	CAN_Transmit(CAN2, &TxMessage);
//}



