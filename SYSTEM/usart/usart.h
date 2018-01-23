#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
	

#define USART_RX_STATUS	0x8000   //数据接收OK
#define USART2_DATA_NUM	(USART2_RX_STA&0X3FFF) //数据长度

/*
串口1的相关定义
*/
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define USART1_REC_LEN  			200  	//定义最大接收字节数 200
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
#define USART1_RXBUFFERSIZE   1 //缓存大小
extern u8 USART1_HalRxBuffer[USART1_RXBUFFERSIZE];//HAL库USART接收Buffer
extern UART_HandleTypeDef UART1_Handler; //UART句柄
extern u16 USART1_RX_STA;         		//接收状态标记	
extern u8 uarst1send(u8 ch);
/*
串口1的相关定义end
*/


/*
串口2的相关定义
*/
#define USART2_REC_LEN  			2048 	//定义最大接收字节数1024   
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
#define USART2_REC_COMMAND_LEN  			200 	//定义最大接收字节数 200
extern u8 USART2_RX_COMMAND[USART2_REC_COMMAND_LEN];     //接收命令反馈的缓冲,最USART2_REC_COMMAND_LEN个字节.
#define USART2_RXBUFFERSIZE   1 //缓存大小
extern u8 USART2_HalRxBuffer[USART2_RXBUFFERSIZE];//HAL库USART接收Buffer
extern u16 CommandReturnNum;//命令返回数据的长度
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口2接收
#define ME3630_USARTX			USART2 				//定义串口2作为4g模块的通信接口
extern u16 USART2_RX_STA;         		//接收状态标记	
//定义向me3630发送一个byte数据
extern u8 me3630SendByte(u8 ch);

//extern u8 CommandSendOkFlag; //标记me3630是否接收命令成功
/*
串口2的相关定义end
*/


/*
串口3的相关定义bound
*/
//#define USART3_BOUND			9600
#define USART3_REC_LEN  			500  	//定义最大接收字节数 200
#define USART3_RXBUFFERSIZE   1 //缓存大小
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口3接收
#define RTK_USARTX			USART3				//定义串口3作为RTK的通信接口

extern u16 gRTK_RX_LEN;//接收的RTK返回的数据的长度
extern u8 gRTK_RX_STATUS;
extern u8 USART3_RX[USART3_REC_LEN];     //接收RTK的数据反馈,USART3_REC_LEN个字节.
//向RTK发送一个byte的数据
extern u8 rtkSendByte(u8 ch);
/*
发送指定长度的数据
*/
extern void UART3_SendDataBytes(char *data,u16 len);



/*
串口3的相关定义end
*/


//如果想串口中断接收，请不要注释以下宏定义
extern void uart_init(u32 bound);

extern void  UART3_Bound(uint32_t bound);

#endif
