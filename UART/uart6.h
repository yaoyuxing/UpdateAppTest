#ifndef __UART4_H__
#define __UART4_H__
#include "sys.h"
#define USART6_RXBUFFERSIZE		1 	//HAL缓存大小
#define EN_UART6_RX 			0
#define USART6_REC_LEN  200 			//定义缓存池的大小
extern u8  gUSART6_RX[USART6_REC_LEN]; 	//数据接收缓存池
extern u16 gUART6_NUM; 				//记录数据更新到那个位置
extern u8  gUSART6_RX_STATUS;


/*
串口6初始化
*/
extern void uart6_init(u32 bound);
/*
串口6发送一个byte数据
*/
extern u8 UART6_SendByte(u8 ch);
/*
串口6发送一个字符串
*/
extern void UART6_SendBytes(char *command);
/*
串口接收数据处理回调函数
*/
/*
发送指定长度的数据
*/
extern void UART6_SendNumBytes(char *data,unsigned int len);

extern void UART6_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART6_HAL_UART_MspInit(UART_HandleTypeDef *huart);
extern void UART6_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
