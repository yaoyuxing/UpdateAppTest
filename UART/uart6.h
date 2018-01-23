#ifndef __UART4_H__
#define __UART4_H__
#include "sys.h"
#define USART6_RXBUFFERSIZE		1 	//HAL�����С
#define EN_UART6_RX 			0
#define USART6_REC_LEN  200 			//���建��صĴ�С
extern u8  gUSART6_RX[USART6_REC_LEN]; 	//���ݽ��ջ����
extern u16 gUART6_NUM; 				//��¼���ݸ��µ��Ǹ�λ��
extern u8  gUSART6_RX_STATUS;


/*
����6��ʼ��
*/
extern void uart6_init(u32 bound);
/*
����6����һ��byte����
*/
extern u8 UART6_SendByte(u8 ch);
/*
����6����һ���ַ���
*/
extern void UART6_SendBytes(char *command);
/*
���ڽ������ݴ���ص�����
*/
/*
����ָ�����ȵ�����
*/
extern void UART6_SendNumBytes(char *data,unsigned int len);

extern void UART6_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART6_HAL_UART_MspInit(UART_HandleTypeDef *huart);
extern void UART6_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
