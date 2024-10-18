#include "debug_usart.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"


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
    while((UART4->SR&0X40)==0); 
    UART4->DR = (u8) ch;  
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


void Debug_Usart4_init(uint32_t bond){
	 //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
 
	//串口4对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); 
	
	//USART4端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bond;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART4, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(UART4, ENABLE);  //使能串口1 
	
	
	
}

































