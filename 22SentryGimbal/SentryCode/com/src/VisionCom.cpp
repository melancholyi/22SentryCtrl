#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "VisionCom.h"
#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif



// 请不要勾选Use MicroLib
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






//缓冲区创建Begin==========//
static uint8_t 	VISION_COM_DMA_Buff		[2][VISION_COM_RECEIVE_BUFF_LENGTH] = {0};  //长度32，每次只接收一个包

 
/*===============END=================*/
/*二、GPIO口初始化这个确实得自己根据自己的GPIO口修改了*/
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
//三、串口初始化
void VISION_COM_Usart_Init(u32 bound)
{  
	
	VISION_COM_UsartGPIO_init();//调用函数一、GPIO口驱动初始化
	
	//定义结构体和时钟initBegin==========//
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    VISION_COM_RCC_ENABLE_USART_FUN;//绝了 今天
	VISION_COM_RCC_ENABLE_DMAX_FUN;//绝了 今天才知道 函数还能宏定义
    /*===============END=================*/
    
	//通信串口初始化Begin==========//
	USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(VISION_COM_USART_PORT, &USART_InitStructure);    
	USART_Cmd(VISION_COM_USART_PORT, ENABLE);	
	USART_ClearFlag(VISION_COM_USART_PORT, USART_FLAG_TC);		
	
	USART_ITConfig(VISION_COM_USART_PORT, USART_IT_IDLE, ENABLE);//开启空闲中断	 这个很重要，可以搜一下stm32空闲中断的说明
	NVIC_InitStructure.NVIC_IRQChannel = VISION_COM_USART_NVIC_IRQ_CHANNEL;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	USART_DMACmd(VISION_COM_USART_PORT,USART_DMAReq_Rx,ENABLE);//开启串口DMA  //开启串口DMA接收 //另外 发送dma我给他拎出去了，一回加回来
	NVIC_InitStructure.NVIC_IRQChannel = VISION_COM_DMA_NVICIRQ_CHANNEL;	//DMA2_Stream2_IRQn 这里的DMA_streamX是从数据手册中查找
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    /*===============END=================*/
    
	//串口DMA接收配置Begin==========//
	DMA_DeInit(VISION_COM_DMA_REC_STREAM);
	while(DMA_GetCmdStatus(VISION_COM_DMA_REC_STREAM) 	!= 	 DISABLE){}//等待DMA可配置 
	DMA_InitStructure.DMA_Channel 						= 					VISION_COM_DMA_REC_CHANNEL;//通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr 			= 	(uint32_t)&(VISION_COM_USART_PORT->DR);//DMA外设地址，对DR进行读操作相当于接收，对DR进行写操作相当于发送
	DMA_InitStructure.DMA_Memory0BaseAddr 				= 	(uint32_t)VISION_COM_DMA_Buff[0];//DMA存储器地址
	DMA_InitStructure.DMA_DIR 							=	 DMA_DIR_PeripheralToMemory;//外设到存储模式
	DMA_InitStructure.DMA_BufferSize = VISION_COM_RECEIVE_BUFF_LENGTH;//数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储区增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度：8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度：8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//使用普通模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//最高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//不使用FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(VISION_COM_DMA_REC_STREAM,&DMA_InitStructure);
	DMA_DoubleBufferModeConfig(VISION_COM_DMA_REC_STREAM, (uint32_t)VISION_COM_DMA_Buff[1], DMA_Memory_0);
	DMA_DoubleBufferModeCmd(VISION_COM_DMA_REC_STREAM, ENABLE);
	DMA_ITConfig(VISION_COM_DMA_REC_STREAM,DMA_IT_TC,ENABLE);		//TC，发送结束中断
	DMA_Cmd(VISION_COM_DMA_REC_STREAM,ENABLE);	
    /*===============END=================*/
		
////***************串口发送的DMA**************************/
	//串口DMA发送配置Begin==========//	
	DMA_InitTypeDef  UsartDMASend_InitStructure;
	DMA_DeInit(VISION_COM_DMA_SEND_STREAM);
	while (DMA_GetCmdStatus(VISION_COM_DMA_SEND_STREAM) != DISABLE){}//等待DMA可配置 
	//配置DMA
	UsartDMASend_InitStructure.DMA_Channel = VISION_COM_DMA_SEND_CHANNEL;                           //通道选择
	UsartDMASend_InitStructure.DMA_PeripheralBaseAddr = (u32)&VISION_COM_USART_PORT->DR;     //DMA外设地址
	UsartDMASend_InitStructure.DMA_Memory0BaseAddr = (u32)VISION_COM_DMA_SEND_BUFFER;               //DMA存储器0地址
	UsartDMASend_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     //存储器到外设模式
	UsartDMASend_InitStructure.DMA_BufferSize = VISION_COM_DMA_SEND_LENGTH;                                    //数据传输量 
	UsartDMASend_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设非增量模式
	UsartDMASend_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //存储器增量模式
	UsartDMASend_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据长度:8位
	UsartDMASend_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //存储器数据长度:8位
	UsartDMASend_InitStructure.DMA_Mode = DMA_Mode_Normal;							//使用普通模式 
	UsartDMASend_InitStructure.DMA_Priority = DMA_Priority_Medium;						//中等优先级
	UsartDMASend_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         							//不使用FIFO
	UsartDMASend_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;								//设置FIFO阈值无效
	UsartDMASend_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;									//存储器突发单次传输
	UsartDMASend_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;					//外设突发单次传输
	DMA_Init(VISION_COM_DMA_SEND_STREAM, &UsartDMASend_InitStructure);																//初始化DMA Stream
	USART_DMACmd(VISION_COM_USART_PORT,USART_DMAReq_Tx,ENABLE);
	DMA_ITConfig(VISION_COM_DMA_SEND_STREAM, DMA_IT_TC, ENABLE);
//    /*===============END=================*/
}


extern "C" void DMA2_Stream2_IRQHandler(void) //麻了麻了 .cpp中的中断函数一定要加 extern "C"
/**串口DMA接收结束后会触发中断，但因为在串口空闲中断中直接中TC位清零，所以不会进去的，但是以防万一还是写上了**/
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
    uint16_t BCCcode = 0;//校验码
    uint16_t handleLoc = 0; //处理缓冲区解包时的定位元素 
	while(buff[handleLoc]!=0XFF)
		handleLoc ++;
	if(buff[handleLoc] == 0xFF && buff[handleLoc+Vision_DataLen-1] == 0x0D ) { //完美情况
		for(int i = handleLoc+1; i <=handleLoc+Vision_DataLen-3; i++)
			BCCcode ^= buff[handleLoc+i]; 
		if(BCCcode == buff[handleLoc+Vision_DataLen-2]){
			//解包
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

	







/** 			第二个版本 串口DMA空闲中断+fifo接受数据
 *@num			2
 *@brief		串口一中断函数 接收中断		
 *@author		choly
 *@time			2021/10/23
 *@param(in)		
 *@returnVal		
 *@attention		
**/

uint16_t errorTimes = 0;


extern "C" void VISION_COM_USART_IRQ_HANDLER()
{
	/************************！！以下！！不需要修改***********************/
	static uint32_t inTestTimes = 0;   //测试数据，记录进入中断的次数 用完记得删
	static uint32_t slovePackTimes = 0;
	uint8_t RecData = 0;	//没啥用 就是清空空闲终端标志位的
	static uint32_t HowManyDataRec = 0;//因为这个变量每次进中断都要用，所以声明为static，不需要每次进来都创建变量，节省一个机器周期
	inTestTimes++;
	//printf("in:%d\n",inTestTimes);
	
	if(USART_GetITStatus(VISION_COM_USART_PORT, USART_IT_IDLE) != RESET) //空闲中断
	{
		
		DMA_Cmd(VISION_COM_DMA_REC_STREAM, DISABLE);		//DMA标志清零
		//先读SR 再读DR，那么就会清除空闲中断标志位
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

MSGrecFromUpper_TypeDef* getUpperCtrlInfo() //得到上位机控制信息
{
	return &UpperCtrlInfo;
}






/*******************************发包函数***************************************/
/**
 *@num					sendPack.1			
 *@brief				初始化 创建发送包		
 *@author				闫创业
 *@time					2021/11/27		
**/
uint8_t sendPacketBuffer[capacity];	//要发送的包

uint8_t* getSendPackBuff_U1()
{
	return sendPacketBuffer;
}

void createPacket(void)
{
	memset(sendPacketBuffer, 0, capacity); //初始化为零
    sendPacketBuffer[0] = 0xff;            //设置帧头
    sendPacketBuffer[capacity - 1] = 0x0d; //设置帧尾
}

/**
 *@num					sendPack.2
 *@brief				清空buffer的数据位 32位情况下是 1-29位	
 *@author				闫创业
 *@time					2021/11/27		
**/
void clearPackData(void)
{
	memset(sendPacketBuffer + 1, 0, capacity - 2);
}

/**
 *@num					sendPack.3
 *@brief				设置校验位		
 *@author				闫创业
 *@time					2021/11/27		
**/
void setCheckCodeBCC()
{
    uint8_t code = 0x00;
    for (int i=1; i<capacity-2; i++)  //将数据位异或
        code ^= sendPacketBuffer[i];
    sendPacketBuffer[capacity - 2] = code;     //设置校验位

}

/**
 *@num					sendPack.4
 *@brief				装载数据		
 *@author				闫创业
 *@time					2021/11/27		
**/

void loadPackData(MSGsendToUpper_TypeDef* sendPack,bool isColor)
{

	memcpy(sendPacketBuffer + 1, &sendPack->packetID,sizeof(sendPack->packetID));
	memcpy(sendPacketBuffer + 5, &sendPack->cmd, sizeof(sendPack->cmd)); 	//四元数测试
	
	if(!isColor){
		memcpy(sendPacketBuffer + 7, &sendPack->shootSpeed, sizeof(sendPack->shootSpeed)); 	//四元数测试
	}
	else{
		memcpy(sendPacketBuffer + 6, &sendPack->myColor, sizeof(sendPack->myColor)); 	//四元数测试
	}
	
	
	
	setCheckCodeBCC();//设置BCC校验位 ！！！！！！一定要加这个 不然解包校验不通过
}

/**
 *@num					sendPack.5
 *@brief				发送函数		
 *@author				闫创业
 *@time					2021/11/27		
**/
void sendPackData_U1()
{
	DMA_Cmd(VISION_COM_DMA_SEND_STREAM, DISABLE);                      //关闭DMA传输 
	while (DMA_GetCmdStatus(VISION_COM_DMA_SEND_STREAM) != DISABLE){}	//确保DMA可以被设置  
	DMA_SetCurrDataCounter(VISION_COM_DMA_SEND_STREAM,VISION_COM_DMA_SEND_LENGTH);          	//数据传输量  
	DMA_Cmd(VISION_COM_DMA_SEND_STREAM, ENABLE);                     	 //开启DMA传输 
	while(1)
	{
		if(DMA_GetFlagStatus(VISION_COM_DMA_SEND_STREAM,DMA_FLAG_TCIF7)!=RESET)
		{
			DMA_ClearFlag(VISION_COM_DMA_SEND_STREAM,DMA_FLAG_TCIF7);//清除通道4传输完成标志
			break;
		}
	}
}

/**
 *@num					sendPack.6
 *@brief				发送函数	u6	
 *@author				闫创业
 *@time					2021/11/27		
**/
//void sendPackData_U6()
//{
//	DMA_Cmd(VISION_COM_DMA_SEND_STREAM_U6, DISABLE);                      //关闭DMA传输 
//	while (DMA_GetCmdStatus(VISION_COM_DMA_SEND_STREAM_U6) != DISABLE){}	//确保DMA可以被设置  
//	DMA_SetCurrDataCounter(VISION_COM_DMA_SEND_STREAM_U6,VISION_COM_DMA_SEND_LENGTH_U6);          	//数据传输量  
//	DMA_Cmd(VISION_COM_DMA_SEND_STREAM_U6, ENABLE);                     	 //开启DMA传输 
//	while(1)
//	{
//		if(DMA_GetFlagStatus(VISION_COM_DMA_SEND_STREAM_U6,VISION_COM_DMA_SEND_TCFLAG_U6)!=RESET)
//		{
//			DMA_ClearFlag(VISION_COM_DMA_SEND_STREAM_U6,VISION_COM_DMA_SEND_TCFLAG_U6);//清除通道4传输完成标志
//			break;
//		}
//	}
//}






















