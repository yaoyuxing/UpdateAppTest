#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
	

#define USART_RX_STATUS	0x8000   //���ݽ���OK
#define USART2_DATA_NUM	(USART2_RX_STA&0X3FFF) //���ݳ���

/*
����1����ض���
*/
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define USART1_REC_LEN  			200  	//�����������ֽ��� 200
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
#define USART1_RXBUFFERSIZE   1 //�����С
extern u8 USART1_HalRxBuffer[USART1_RXBUFFERSIZE];//HAL��USART����Buffer
extern UART_HandleTypeDef UART1_Handler; //UART���
extern u16 USART1_RX_STA;         		//����״̬���	
extern u8 uarst1send(u8 ch);
/*
����1����ض���end
*/


/*
����2����ض���
*/
#define USART2_REC_LEN  			2048 	//�����������ֽ���1024   
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
#define USART2_REC_COMMAND_LEN  			200 	//�����������ֽ��� 200
extern u8 USART2_RX_COMMAND[USART2_REC_COMMAND_LEN];     //����������Ļ���,��USART2_REC_COMMAND_LEN���ֽ�.
#define USART2_RXBUFFERSIZE   1 //�����С
extern u8 USART2_HalRxBuffer[USART2_RXBUFFERSIZE];//HAL��USART����Buffer
extern u16 CommandReturnNum;//��������ݵĳ���
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������2����
#define ME3630_USARTX			USART2 				//���崮��2��Ϊ4gģ���ͨ�Žӿ�
extern u16 USART2_RX_STA;         		//����״̬���	
//������me3630����һ��byte����
extern u8 me3630SendByte(u8 ch);

//extern u8 CommandSendOkFlag; //���me3630�Ƿ��������ɹ�
/*
����2����ض���end
*/


/*
����3����ض���bound
*/
//#define USART3_BOUND			9600
#define USART3_REC_LEN  			500  	//�����������ֽ��� 200
#define USART3_RXBUFFERSIZE   1 //�����С
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������3����
#define RTK_USARTX			USART3				//���崮��3��ΪRTK��ͨ�Žӿ�

extern u16 gRTK_RX_LEN;//���յ�RTK���ص����ݵĳ���
extern u8 gRTK_RX_STATUS;
extern u8 USART3_RX[USART3_REC_LEN];     //����RTK�����ݷ���,USART3_REC_LEN���ֽ�.
//��RTK����һ��byte������
extern u8 rtkSendByte(u8 ch);
/*
����ָ�����ȵ�����
*/
extern void UART3_SendDataBytes(char *data,u16 len);



/*
����3����ض���end
*/


//����봮���жϽ��գ��벻Ҫע�����º궨��
extern void uart_init(u32 bound);

extern void  UART3_Bound(uint32_t bound);

#endif
