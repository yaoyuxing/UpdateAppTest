#ifndef __UART7_H__
#define __UART7_H__
#include "sys.h"
#define USART7_RXBUFFERSIZE		1 	//HAL缓存大小
#define EN_UART7_RX 			1
#define USART7_REC_LEN  200 			//定义缓存池的大小
extern u8 	gUSART7_RX[USART7_REC_LEN]; 	//数据接收缓存池
extern u16 	gUART7_NUM; 				//记录数据更新到那个位置
extern u8 	gUSART7_RX_STATUS;

/*
串口4初始化
*/
extern void UART7_init(u32 bound);
/*
串口4发送一个byte数据
*/
extern u8 UART7_SendByte(u8 ch);
/*
串口4发送一个字符串
*/
extern void UART7_SendBytes(char *command);
/*
串口7发送一段内存的数据
*/
extern void UART7_SendBytesSizeof(char *command);
/*
串口7发送一个字符串
*/
extern void UART7_SendBytesLen(char *data ,uint16_t len);
/*
修改波特率函数
*/
extern void UART7_Bound(u32 bound);
/*
串口接收数据处理回调函数
*/

extern void UART7_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART7_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
