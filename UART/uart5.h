#ifndef __UART5_H__
#define __UART5_H__
#include "sys.h"

#define USART5_DEVELOPMENT_BOARD		0 	//==1��ʾ�ǿ�����

#define USART5_RXBUFFERSIZE		1 	//HAL�����С
#define EN_UART5_RX 			1
#define USART5_REC_LEN  512 			//���建��صĴ�С
extern u8 gUSART5_RX[USART5_REC_LEN]; 	//���ݽ��ջ����
extern u16 gUART5_NUM; 				//��¼���ݸ��µ��Ǹ�λ��
extern u8 gUSART5_RX_STATUS;


/*
����5��ʼ��
*/
extern void uart5_init(u32 bound);
/*
����5����һ��byte����
*/
extern u8 UART5_SendByte(u8 ch);
/*
����5����һ���ַ���
*/
extern void UART5_SendBytes(char *command);


/*
����ָ�����ȵ�����
*/
void UART5_SendDataBytes(char *data,u16 len);
/*
����5����һ���ڴ������
*/
void UART5_SendBytesSizeof(char *data);
/*
���ڽ������ݴ���ص�����
*/


extern void UART5_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART5_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
