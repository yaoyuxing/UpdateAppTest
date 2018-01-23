#ifndef __UART4_H__
#define __UART4_H__
#include "sys.h"
#define USART4_RXBUFFERSIZE		1 	//HAL�����С
#define EN_UART4_RX 			1
#define USART4_REC_LEN  20 			//���建��صĴ�С
extern u8  gUSART4_RX[USART4_REC_LEN]; 	//���ݽ��ջ����
extern u16 gUART4_NUM; 				//��¼���ݸ��µ��Ǹ�λ��
extern u8  gUSART4_RX_STATUS;


/*
����4��ʼ��
*/
extern void uart4_init(u32 bound);
/*
����4����һ��byte����
*/
extern u8 UART4_SendByte(u8 ch);
/*
����4����һ���ַ���
*/
extern void UART4_SendBytes(char *command);
/*
����4����ָ�����ȵ�����
*/
extern void UART4_SendNumBytes(char *data,unsigned int len);

/*
���ڽ������ݴ���ص�����
*/

extern void UART4_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void UART4_HAL_UART_MspInit(UART_HandleTypeDef *huart);
#endif
