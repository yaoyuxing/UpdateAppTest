#ifndef __UART4_H__
#define __UART4_H__
#include "sys.h"
#define USART4_RXBUFFERSIZE		1 	//HAL缓存大小
#define EN_UART4_RX 			1
#define USART4_REC_LEN  20 			//定义缓存池的大小
extern u8  gUSART4_RX[USART4_REC_LEN]; 	//数据接收缓存池
extern u16 gUART4_NUM; 				//记录数据更新到那个位置
extern u8  gUSART4_RX_STATUS;


/*
串口4初始化
*/
extern void uart4_init(u32 bound);
/*
串口4发送一个byte数据
*/
extern u8 UART4_SendByte(u8 ch);
/*
串口4发送一个字符串
*/
extern void UART4_SendBytes(char *command);
/*
串口4发送指定长度的数据
*/
extern void UART4_SendNumBytes(char *data,unsigned int len);

/*
串口接收数据处理回调函数
*/

extern void UART4_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART4_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
