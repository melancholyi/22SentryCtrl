#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "VisionCom.h"
#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif



// �벻Ҫ��ѡUse MicroLib
#ifdef __MICROLIB
#error "Please do not use MicroLib!"
#endif

	
__ASM (".global __use_no_semihosting");  
    
	
struct __FILE {
    int handle; 
}; 


FILE __stdout; 
FILE __stdin;
FILE __stderr;

int fputc(int ch, FILE *f){
    while((USART6->SR&0X40)==0); 
    USART6->DR = (u8) ch;  
    return ch;
}
void _sys_exit(int x){
    x = x; 
}
void _ttywrch(int ch) {ch=ch;}

typedef int FILEHANLE;
FILEHANLE _sys_open(const char * name, int openmode){
	return 1;
}

#ifdef __cplusplus
}
#endif
/*===============END=================*/






//����������Begin==========//
static uint8_t 	VISION_COM_DMA_Buff		[2][VISION_COM_RECEIVE_BUFF_LENGTH] = {0};  //����32��ÿ��ֻ����һ����

 
/*===============END=================*/
/*����GPIO�ڳ�ʼ�����ȷʵ���Լ������Լ���GPIO���޸���*/
void VISION_COM_UsartGPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	  
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
}

DMA_InitTypeDef	DMA_InitStructure;
//�������ڳ�ʼ��
void VISION_COM_Usart_Init(u32 bound)
{  
	
	VISION_COM_UsartGPIO_init();//���ú���һ��GPIO��������ʼ��
	
	//����ṹ���ʱ��initBegin==========//
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    VISION_COM_RCC_ENABLE_USART_FUN;//���� ����
	VISION_COM_RCC_ENABLE_DMAX_FUN;//���� �����֪�� �������ܺ궨��
    /*===============END=================*/
    
	//ͨ�Ŵ��ڳ�ʼ��Begin==========//
	USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(VISION_COM_USART_PORT, &USART_InitStructure);    
	USART_Cmd(VISION_COM_USART_PORT, ENABLE);	
	USART_ClearFlag(VISION_COM_USART_PORT, USART_FLAG_TC);		
	
	USART_ITConfig(VISION_COM_USART_PORT, USART_IT_IDLE, ENABLE);//���������ж�	 �������Ҫ��������һ��stm32�����жϵ�˵��
	NVIC_InitStructure.NVIC_IRQChannel = VISION_COM_USART_NVIC_IRQ_CHANNEL;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	USART_DMACmd(VISION_COM_USART_PORT,USART_DMAReq_Rx,ENABLE);//��������DMA  //��������DMA���� //���� ����dma�Ҹ������ȥ�ˣ�һ�ؼӻ���
	NVIC_InitStructure.NVIC_IRQChannel = VISION_COM_DMA_NVICIRQ_CHANNEL;	//DMA2_Stream2_IRQn �����DMA_streamX�Ǵ������ֲ��в���
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    /*===============END=================*/
    
	//����DMA��������Begin==========//
	DMA_DeInit(VISION_COM_DMA_REC_STREAM);
	while(DMA_GetCmdStatus(VISION_COM_DMA_REC_STREAM) 	!= 	 DISABLE){}//�ȴ�DMA������ 
	DMA_InitStructure.DMA_Channel 						= 					VISION_COM_DMA_REC_CHANNEL;//ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr 			= 	(uint32_t)&(VISION_COM_USART_PORT->DR);//DMA�����ַ����DR���ж������൱�ڽ��գ���DR����д�����൱�ڷ���
	DMA_InitStructure.DMA_Memory0BaseAddr 				= 	(uint32_t)VISION_COM_DMA_Buff[0];//DMA�洢����ַ
	DMA_InitStructure.DMA_DIR 							=	 DMA_DIR_PeripheralToMemory;//���赽�洢ģʽ
	DMA_InitStructure.DMA_BufferSize = VISION_COM_RECEIVE_BUFF_LENGTH;//���ݴ�����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ��ȣ�8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ��ȣ�8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//ʹ����ͨģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//������ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//��ʹ��FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
	DMA_Init(VISION_COM_DMA_REC_STREAM,&DMA_InitStructure);
	DMA_DoubleBufferModeConfig(VISION_COM_DMA_REC_STREAM, (uint32_t)VISION_COM_DMA_Buff[1], DMA_Memory_0);
	DMA_DoubleBufferModeCmd(VISION_COM_DMA_REC_STREAM, ENABLE);
	DMA_ITConfig(VISION_COM_DMA_REC_STREAM,DMA_IT_TC,ENABLE);		//TC�����ͽ����ж�
	DMA_Cmd(VISION_COM_DMA_REC_STREAM,ENABLE);	
    /*===============END=================*/
		
////***************���ڷ��͵�DMA**************************/
	//����DMA��������Begin==========//	
	DMA_InitTypeDef  UsartDMASend_InitStructure;
	DMA_DeInit(VISION_COM_DMA_SEND_STREAM);
	while (DMA_GetCmdStatus(VISION_COM_DMA_SEND_STREAM) != DISABLE){}//�ȴ�DMA������ 
	//����DMA
	UsartDMASend_InitStructure.DMA_Channel = VISION_COM_DMA_SEND_CHANNEL;                           //ͨ��ѡ��
	UsartDMASend_InitStructure.DMA_PeripheralBaseAddr = (u32)&VISION_COM_USART_PORT->DR;     //DMA�����ַ
	UsartDMASend_InitStructure.DMA_Memory0BaseAddr = (u32)VISION_COM_DMA_SEND_BUFFER;               //DMA�洢��0��ַ
	UsartDMASend_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     //�洢��������ģʽ
	UsartDMASend_InitStructure.DMA_BufferSize = VISION_COM_DMA_SEND_LENGTH;                                    //���ݴ����� 
	UsartDMASend_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //���������ģʽ
	UsartDMASend_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //�洢������ģʽ
	UsartDMASend_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //�������ݳ���:8λ
	UsartDMASend_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //�洢�����ݳ���:8λ
	UsartDMASend_InitStructure.DMA_Mode = DMA_Mode_Normal;							//ʹ����ͨģʽ 
	UsartDMASend_InitStructure.DMA_Priority = DMA_Priority_Medium;						//�е����ȼ�
	UsartDMASend_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         							//��ʹ��FIFO
	UsartDMASend_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;								//����FIFO��ֵ��Ч
	UsartDMASend_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;									//�洢��ͻ�����δ���
	UsartDMASend_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;					//����ͻ�����δ���
	DMA_Init(VISION_COM_DMA_SEND_STREAM, &UsartDMASend_InitStructure);																//��ʼ��DMA Stream
	USART_DMACmd(VISION_COM_USART_PORT,USART_DMAReq_Tx,ENABLE);
	DMA_ITConfig(VISION_COM_DMA_SEND_STREAM, DMA_IT_TC, ENABLE);
//    /*===============END=================*/
}


extern "C" void DMA2_Stream2_IRQHandler(void) //�������� .cpp�е��жϺ���һ��Ҫ�� extern "C"
/**����DMA���ս�����ᴥ���жϣ�����Ϊ�ڴ��ڿ����ж���ֱ����TCλ���㣬���Բ����ȥ�ģ������Է���һ����д����**/
{ 
	if(DMA_GetITStatus(VISION_COM_DMA_REC_STREAM,VISION_COM_DMA_IT_TCIF))
	{
		if(DMA_GetFlagStatus(VISION_COM_DMA_REC_STREAM,VISION_COM_DMA_FLAG_TCIF) )
		{
			DMA_ClearFlag(VISION_COM_DMA_REC_STREAM,VISION_COM_DMA_FLAG_TCIF);
		}
		DMA_ClearITPendingBit(VISION_COM_DMA_REC_STREAM,VISION_COM_DMA_IT_TCIF);
	}
}

MSGrecFromUpper_TypeDef UpperCtrlInfo;

void slovePacket(uint8_t* buff){
    uint16_t BCCcode = 0;//У����
    uint16_t handleLoc = 0; //�����������ʱ�Ķ�λԪ�� 
	while(buff[handleLoc]!=0XFF)
		handleLoc ++;
	if(buff[handleLoc] == 0xFF && buff[handleLoc+Vision_DataLen-1] == 0x0D ) { //�������
		for(int i = handleLoc+1; i <=handleLoc+Vision_DataLen-3; i++)
			BCCcode ^= buff[handleLoc+i]; 
		if(BCCcode == buff[handleLoc+Vision_DataLen-2]){
			//���
			memcpy(&UpperCtrlInfo.packetID,buff+handleLoc+1,4);
			if(*(buff+handleLoc+5) == 0x2a) UpperCtrlInfo.sentryMode = sentry_autoAim_e;
			else if (*(buff+handleLoc+5) == 0x3a) UpperCtrlInfo.sentryMode = sentry_scout_e;
			else if (*(buff+handleLoc+5) == 0x4a) UpperCtrlInfo.sentryMode = sentry_Dbus_e;
			else	UpperCtrlInfo.sentryMode = snetry_noCmd_e;
			
			memcpy(&UpperCtrlInfo.isShoot,buff+handleLoc+6,1);
			memcpy(&UpperCtrlInfo.yawTarAngel,buff+handleLoc+7,4);
			memcpy(&UpperCtrlInfo.pitchTarAngel,buff+handleLoc+11,4);
			memcpy(&UpperCtrlInfo.enemyNum,buff+handleLoc+16,1);
			
//			printf("ID:%d,%d\n",
//			 			UpperCtrlInfo.packetID,
//			 			UpperCtrlInfo.isShoot
//			 );	
//			
			
			
//			
//			 printf("ID:%d\t Mode:%d\t Tyaw:%.2f\t Tpitch:%.2f\n",
//			 			UpperCtrlInfo.packetID,
//			 			UpperCtrlInfo.sentryMode,
//			 			UpperCtrlInfo.yawTarAngel,
//			 			UpperCtrlInfo.pitchTarAngel
//			 );	

//			printf("yawPit:%.2f,%.2f\n",
//			 			UpperCtrlInfo.yawTarAngel,
//			 			UpperCtrlInfo.pitchTarAngel
//			 );		
			
			
//			 printf("ID:%d,%.2f\n",
//			 			UpperCtrlInfo.packetID,
//			 			UpperCtrlInfo.pitchTarAngel
//			 );				
		}
		else printf("check code mistakes!!\r\n");
	}
	else printf("no find frame head and tail!!\r\n");
}

	







/** 			�ڶ����汾 ����DMA�����ж�+fifo��������
 *@num			2
 *@brief		����һ�жϺ��� �����ж�		
 *@author		choly
 *@time			2021/10/23
 *@param(in)		
 *@returnVal		
 *@attention		
**/

uint16_t errorTimes = 0;


extern "C" void VISION_COM_USART_IRQ_HANDLER()
{
	/************************�������£�������Ҫ�޸�***********************/
	static uint32_t inTestTimes = 0;   //�������ݣ���¼�����жϵĴ��� ����ǵ�ɾ
	static uint32_t slovePackTimes = 0;
	uint8_t RecData = 0;	//ûɶ�� ������տ����ն˱�־λ��
	static uint32_t HowManyDataRec = 0;//��Ϊ�������ÿ�ν��ж϶�Ҫ�ã���������Ϊstatic������Ҫÿ�ν�����������������ʡһ����������
	inTestTimes++;
	//printf("in:%d\n",inTestTimes);
	
	if(USART_GetITStatus(VISION_COM_USART_PORT, USART_IT_IDLE) != RESET) //�����ж�
	{
		
		DMA_Cmd(VISION_COM_DMA_REC_STREAM, DISABLE);		//DMA��־����
		//�ȶ�SR �ٶ�DR����ô�ͻ���������жϱ�־λ
		RecData = VISION_COM_USART_PORT->SR;//
		RecData = VISION_COM_USART_PORT->DR;//
        if(DMA_GetCurrentMemoryTarget(VISION_COM_DMA_REC_STREAM) == 0){
            DMA_Cmd(VISION_COM_DMA_REC_STREAM,DISABLE);
            DMA_SetCurrDataCounter(VISION_COM_DMA_REC_STREAM,VISION_COM_RECEIVE_BUFF_LENGTH);
            DMA_DoubleBufferModeConfig(VISION_COM_DMA_REC_STREAM,(uint32_t)VISION_COM_DMA_Buff[1],DMA_Memory_1);
            DMA_Cmd(VISION_COM_DMA_REC_STREAM,ENABLE);
			//printf("len:%d\r\n",DMA_GetCurrDataCounter(VISION_COM_DMA_REC_STREAM));
            if(DMA_GetCurrDataCounter(VISION_COM_DMA_REC_STREAM) == Vision_DataLen+8){
				slovePackTimes++;
                //printf("slove buff1 intimes:%d sloveTimes:%d\r\n",inTestTimes,slovePackTimes);
                slovePacket(VISION_COM_DMA_Buff[0]);
            }
        }
        else {
            DMA_Cmd(VISION_COM_DMA_REC_STREAM,DISABLE);
            DMA_SetCurrDataCounter(VISION_COM_DMA_REC_STREAM,VISION_COM_RECEIVE_BUFF_LENGTH);
            DMA_DoubleBufferModeConfig(VISION_COM_DMA_REC_STREAM,(uint32_t)VISION_COM_DMA_Buff[0],DMA_Memory_0);
            DMA_Cmd(VISION_COM_DMA_REC_STREAM,ENABLE);
			//printf("len:%d\r\n",DMA_GetCurrDataCounter(VISION_COM_DMA_REC_STREAM));
            if(DMA_GetCurrDataCounter(VISION_COM_DMA_REC_STREAM) == Vision_DataLen+8){
				slovePackTimes++;
                //printf("slove buff2 intimes:%d sloveTimes:%d\r\n",inTestTimes,slovePackTimes);
                slovePacket(VISION_COM_DMA_Buff[1]);
            }
        }
	}
}

MSGrecFromUpper_TypeDef* getUpperCtrlInfo() //�õ���λ��������Ϣ
{
	return &UpperCtrlInfo;
}






/*******************************��������***************************************/
/**
 *@num					sendPack.1			
 *@brief				��ʼ�� �������Ͱ�		
 *@author				�ƴ�ҵ
 *@time					2021/11/27		
**/
uint8_t sendPacketBuffer[capacity];	//Ҫ���͵İ�

uint8_t* getSendPackBuff_U1()
{
	return sendPacketBuffer;
}

void createPacket(void)
{
	memset(sendPacketBuffer, 0, capacity); //��ʼ��Ϊ��
    sendPacketBuffer[0] = 0xff;            //����֡ͷ
    sendPacketBuffer[capacity - 1] = 0x0d; //����֡β
}

/**
 *@num					sendPack.2
 *@brief				���buffer������λ 32λ������� 1-29λ	
 *@author				�ƴ�ҵ
 *@time					2021/11/27		
**/
void clearPackData(void)
{
	memset(sendPacketBuffer + 1, 0, capacity - 2);
}

/**
 *@num					sendPack.3
 *@brief				����У��λ		
 *@author				�ƴ�ҵ
 *@time					2021/11/27		
**/
void setCheckCodeBCC()
{
    uint8_t code = 0x00;
    for (int i=1; i<capacity-2; i++)  //������λ���
        code ^= sendPacketBuffer[i];
    sendPacketBuffer[capacity - 2] = code;     //����У��λ

}

/**
 *@num					sendPack.4
 *@brief				װ������		
 *@author				�ƴ�ҵ
 *@time					2021/11/27		
**/

void loadPackData(MSGsendToUpper_TypeDef* sendPack,bool isColor)
{

	memcpy(sendPacketBuffer + 1, &sendPack->packetID,sizeof(sendPack->packetID));
	memcpy(sendPacketBuffer + 5, &sendPack->cmd, sizeof(sendPack->cmd)); 	//��Ԫ������
	
	if(!isColor){
		memcpy(sendPacketBuffer + 7, &sendPack->shootSpeed, sizeof(sendPack->shootSpeed)); 	//��Ԫ������
	}
	else{
		memcpy(sendPacketBuffer + 6, &sendPack->myColor, sizeof(sendPack->myColor)); 	//��Ԫ������
	}
	
	
	
	setCheckCodeBCC();//����BCCУ��λ ������������һ��Ҫ����� ��Ȼ���У�鲻ͨ��
}

/**
 *@num					sendPack.5
 *@brief				���ͺ���		
 *@author				�ƴ�ҵ
 *@time					2021/11/27		
**/
void sendPackData_U1()
{
	DMA_Cmd(VISION_COM_DMA_SEND_STREAM, DISABLE);                      //�ر�DMA���� 
	while (DMA_GetCmdStatus(VISION_COM_DMA_SEND_STREAM) != DISABLE){}	//ȷ��DMA���Ա�����  
	DMA_SetCurrDataCounter(VISION_COM_DMA_SEND_STREAM,VISION_COM_DMA_SEND_LENGTH);          	//���ݴ�����  
	DMA_Cmd(VISION_COM_DMA_SEND_STREAM, ENABLE);                     	 //����DMA���� 
	while(1)
	{
		if(DMA_GetFlagStatus(VISION_COM_DMA_SEND_STREAM,DMA_FLAG_TCIF7)!=RESET)
		{
			DMA_ClearFlag(VISION_COM_DMA_SEND_STREAM,DMA_FLAG_TCIF7);//���ͨ��4������ɱ�־
			break;
		}
	}
}

/**
 *@num					sendPack.6
 *@brief				���ͺ���	u6	
 *@author				�ƴ�ҵ
 *@time					2021/11/27		
**/
//void sendPackData_U6()
//{
//	DMA_Cmd(VISION_COM_DMA_SEND_STREAM_U6, DISABLE);                      //�ر�DMA���� 
//	while (DMA_GetCmdStatus(VISION_COM_DMA_SEND_STREAM_U6) != DISABLE){}	//ȷ��DMA���Ա�����  
//	DMA_SetCurrDataCounter(VISION_COM_DMA_SEND_STREAM_U6,VISION_COM_DMA_SEND_LENGTH_U6);          	//���ݴ�����  
//	DMA_Cmd(VISION_COM_DMA_SEND_STREAM_U6, ENABLE);                     	 //����DMA���� 
//	while(1)
//	{
//		if(DMA_GetFlagStatus(VISION_COM_DMA_SEND_STREAM_U6,VISION_COM_DMA_SEND_TCFLAG_U6)!=RESET)
//		{
//			DMA_ClearFlag(VISION_COM_DMA_SEND_STREAM_U6,VISION_COM_DMA_SEND_TCFLAG_U6);//���ͨ��4������ɱ�־
//			break;
//		}
//	}
//}






















