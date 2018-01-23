#ifndef __UART5_H__
#define __UART5_H__
#include "sys.h"

#define USART5_DEVELOPMENT_BOARD		0 	//==1表示是开发板

#define USART5_RXBUFFERSIZE		1 	//HAL缓存大小
#define EN_UART5_RX 			1
#define USART5_REC_LEN  512 			//定义缓存池的大小
extern u8 gUSART5_RX[USART5_REC_LEN]; 	//数据接收缓存池
extern u16 gUART5_NUM; 				//记录数据更新到那个位置
extern u8 gUSART5_RX_STATUS;


/*
串口5初始化
*/
extern void uart5_init(u32 bound);
/*
串口5发送一个byte数据
*/
extern u8 UART5_SendByte(u8 ch);
/*
串口5发送一个字符串
*/
extern void UART5_SendBytes(char *command);


/*
发送指定长度的数据
*/
void UART5_SendDataBytes(char *data,u16 len);
/*
串口5发送一段内存的数据
*/
void UART5_SendBytesSizeof(char *data);
/*
串口接收数据处理回调函数
*/


extern void UART5_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART5_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
