#ifndef __UART7_H__
#define __UART7_H__
#include "sys.h"
#define USART7_RXBUFFERSIZE		1 	//HAL�����С
#define EN_UART7_RX 			1
#define USART7_REC_LEN  200 			//���建��صĴ�С
extern u8 	gUSART7_RX[USART7_REC_LEN]; 	//���ݽ��ջ����
extern u16 	gUART7_NUM; 				//��¼���ݸ��µ��Ǹ�λ��
extern u8 	gUSART7_RX_STATUS;

/*
����4��ʼ��
*/
extern void UART7_init(u32 bound);
/*
����4����һ��byte����
*/
extern u8 UART7_SendByte(u8 ch);
/*
����4����һ���ַ���
*/
extern void UART7_SendBytes(char *command);
/*
����7����һ���ڴ������
*/
extern void UART7_SendBytesSizeof(char *command);
/*
����7����һ���ַ���
*/
extern void UART7_SendBytesLen(char *data ,uint16_t len);
/*
�޸Ĳ����ʺ���
*/
extern void UART7_Bound(u32 bound);
/*
���ڽ������ݴ���ص�����
*/

extern void UART7_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART7_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
