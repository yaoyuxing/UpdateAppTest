#ifndef USART_H
#define USART_H
#include "stm32f7xx_hal.h"



#define USE_USART1 
#define USE_USART2
//#define USE_USART3 
//#define USE_UART4
#define USE_UART5
//#define USE_USART6
//#define USE_UART7  
#define UART_DEBUG 



//#define USART_TEST

#ifdef  USART_TEST
void usart_test_fun(void);
#endif



#define  	ESCAPE_CHAR	  				0x7d //ת���ַ�
#define  	XOR_CHAR		 			 0x20	//���ֵ 
#define     UART_FLAG_MAX              5
//��־ƫ��
#define 	UART_RX_FRAME_FLAG_POS        0
#define 	UART_RX_BYTE_FLAG_POS        1
#define     UART_RX_START_FLAG_POS       2



#define  UART_RX_BUFFER_SIZE  1024
typedef enum usart_rx_mode
{
	UART_RX_FRAME_MODE=0,
	UART_RX_LEN_MODE=1,
	UART_RX_NORMAL_MODE=2, 
}eUART_RxModeType;
typedef struct usart_base_type
{ 
	UART_HandleTypeDef UartHandle;      //���տ��ƾ��
	eUART_RxModeType eRxMode;          //����ģʽ
	unsigned char    ucRecByte;        //��ǰ���յ����ֽ�
	unsigned char    Buffer[UART_RX_BUFFER_SIZE];           //������
	unsigned int     unBufferLen;       //ʵ�ʽ��ճ���
	unsigned  int    unRxSetLen;       //���ý��ճ��� 
volatile    unsigned  char   Flags[UART_FLAG_MAX]; 
}stUART_BaseType;
typedef struct uart_rx_ctrl
{
	unsigned int  Uartx;  
	unsigned char *pRecFrameFlag;    //��־����֡���
	unsigned char *pRecByteFlag;     //��־���յ����ֽ����
	unsigned char *pBuffer;         //ָ�򻺳���
	unsigned int  *pBufferLen;  	//ָ����ճ��� 
	unsigned char *pReByte;         //ָ����յĵ����ֽ�
	eUART_RxModeType *pMode;       	//ģʽ
	char cHead;						//ͷ
	char cTail;						//β
	unsigned char ucUseEscape;      //��ת��
	unsigned char ucEscFlag;        //����ת���־
	unsigned char *pFrameStartFlag; //ָ��֡��ʼ��־
	unsigned int  *pRxSetLen;       //ָ�����ý������ݳ���
}stUART_RxCtrlType;









#ifdef USE_USART1

#define   USART1_CLK_ENABLE()    __HAL_RCC_USART1_CLK_ENABLE()
#define   USART1_TX_CLK_ENBALE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define   USART1_RX_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define   USART1_TX_PIN          GPIO_PIN_9
#define   USART1_TX_AF           GPIO_AF7_USART1
#define   USART1_TX_PORT	       GPIOA
#define   USART1_RX_PIN          GPIO_PIN_10
#define   USART1_RX_AF           GPIO_AF7_USART1
#define   USART1_RX_PORT         GPIOA

//head tail
#define  UART1_HEAD_CHAR          '#'
#define  UART1_TAIL_CHAR          '@'

#define UART1_RX_GOT_FRAME_FLAG    (gstUart1.Flags[UART_RX_FRAME_FLAG_POS])
#define UART1_RX_GOT_BYTE_FLAG      (gstUart1.Flags[UART_RX_BYTE_FLAG_POS]) 
#define UART1_RX_FRAME_START_FLAG     (gstUart1.Flags[UART_RX_START_FLAG_POS]) 
extern  stUART_BaseType   gstUart1;
#define COM1_SendBytes(pdata,size)       do{ HAL_UART_Transmit(&(gstUart1.UartHandle),pdata,size,UART_TX_TIME_OUT);}while(0)
#define COM1_RecBytes(pdata,size)        do{HAL_UART_Receive_IT(&(gstUart1.UartHandle),pdata,size);}while(0)
#define UART1_SendString(pStr)           SendString(&(gstUart1.UartHandle),pStr)
void UART1_Printf(const char *fmt,...);
#endif





/**************************                        ���� 2                               ******************************/
#ifdef USE_USART2

#define   USART2_CLK_ENABLE()    __HAL_RCC_USART2_CLK_ENABLE()
#define   USART2_TX_CLK_ENBALE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define   USART2_RX_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define   USART2_TX_PIN          GPIO_PIN_2
#define   USART2_TX_AF           GPIO_AF7_USART2
#define   USART2_TX_PORT	     GPIOA
#define   USART2_RX_PIN          GPIO_PIN_3
#define   USART2_RX_AF           GPIO_AF7_USART2
#define   USART2_RX_PORT         GPIOA
//head tail
#define  UART2_HEAD_CHAR          '#'
#define  UART2_TAIL_CHAR          '@'

#define UART2_RX_GOT_FRAME_FLAG    		(gstUart2.Flags[UART_RX_FRAME_FLAG_POS])
#define UART2_RX_GOT_BYTE_FLAG      	(gstUart2.Flags[UART_RX_BYTE_FLAG_POS]) 
#define UART2_RX_FRAME_START_FLAG    	 (gstUart2.Flags[UART_RX_START_FLAG_POS]) 
extern  stUART_BaseType   gstUart2;
#define COM2_SendBytes(pdata,size)       do{ HAL_UART_Transmit(&(gstUart2.UartHandle),pdata,size,UART_TX_TIME_OUT);}while(0)
#define COM2_RecBytes(pdata,size)        do{HAL_UART_Receive_IT(&(gstUart2.UartHandle),pdata,size);}while(0)
#define UART2_SendString(pStr)           SendString(&(gstUart2.UartHandle),pStr)
#endif
/*********************************************************************************************************************/
 

#ifdef USE_USART3
extern  stUART_BaseType   gstUart3;
#define COM3_SendBytes(pdata,size)       do{ HAL_UART_Transmit(&(gstUart3.UartHandle),pdata,size,UART_TX_TIME_OUT);}while(0)
#define COM3_RecBytes(pdata,size)        do{HAL_UART_Receive_IT(&(gstUart3.UartHandle),pdata,size);}while(0)
#define  UART3_SendString(pStr)          SendString(&(gstUart3.UartHandle),pStr)
#endif
#ifdef USE_UART4
extern  stUART_BaseType   gstUart4;
#define COM4_SendBytes(pdata,size)       do{ HAL_UART_Transmit(&(gstUart4.UartHandle),pdata,size,UART_TX_TIME_OUT);}while(0)
#define COM4_RecBytes(pdata,size)        do{HAL_UART_Receive_IT(&(gstUart4.UartHandle),pdata,size);}while(0)
#define  UART4_SendString(pStr)          SendString(&(gstUart4.UartHandle),pStr)
#endif
/*********************************************************************************************************************/



/**************************                        ���� 5                               ******************************/
#ifdef USE_UART5
//IO ����
#define   UART5_CLK_ENABLE()    __HAL_RCC_UART5_CLK_ENABLE()
#define   UART5_TX_CLK_ENBALE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define   UART5_RX_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define   UART5_TX_PIN          GPIO_PIN_13
#define   UART5_TX_AF           GPIO_AF8_UART5
#define   UART5_TX_PORT	        GPIOB
#define   UART5_RX_PIN          GPIO_PIN_12
#define   UART5_RX_AF           GPIO_AF8_UART5
#define   UART5_RX_PORT         GPIOB
//head tail
#define  UART5_HEAD_CHAR          '#'
#define  UART5_TAIL_CHAR          '@'

//���ջ��������� 
#define UART5_RX_GOT_FRAME_FLAG    (gstUart5.Flags[UART_RX_FRAME_FLAG_POS])
#define UART5_RX_GOT_BYTE_FLAG      (gstUart5.Flags[UART_RX_BYTE_FLAG_POS]) 
#define UART5_RX_FRAME_START_FLAG     (gstUart5.Flags[UART_RX_START_FLAG_POS]) 
extern  stUART_BaseType   gstUart5;
#define COM5_SendBytes(pdata,size)       do{ HAL_UART_Transmit(&(gstUart5.UartHandle),pdata,size,UART_TX_TIME_OUT);}while(0)
#define COM5_RecBytes(pdata,size)        do{HAL_UART_Receive_IT(&(gstUart5.UartHandle),pdata,size);}while(0)
#define  UART5_SendString(pStr)          SendString(&(gstUart5.UartHandle),pStr)

#endif
/*********************************************************************************************************************/



#ifdef USE_USART6
extern  stUART_BaseType   gstUart6;
#define COM6_SendBytes(pdata,size)       do{ HAL_UART_Transmit(&(gstUart6.UartHandle),pdata,size,UART_TX_TIME_OUT);}while(0)
#define COM6_RecBytes(pdata,size)        do{HAL_UART_Receive_IT(&(gstUart6.UartHandle),pdata,size);}while(0)
#define  UART6_SendString(pStr)          SendString(&(gstUart6.UartHandle),pStr)
#endif
#ifdef USE_UART7

extern  stUART_BaseType   gstUart7;
#define COM7_SendBytes(pdata,size)       do{ HAL_UART_Transmit(&(gstUart7.UartHandle),pdata,size,UART_TX_TIME_OUT);}while(0)
#define COM7_RecBytes(pdata,size)        do{HAL_UART_Receive_IT(&(gstUart7.UartHandle),pdata,size);}while(0)
#define  UART7_SendString(pStr)          SendString(&(gstUart7.UartHandle),pStr)
#endif

/**************************                        ���� ����                               ******************************/

#define   UART_TX_TIME_OUT          HAL_MAX_DELAY
//#define   UART_RX_TIME_OUT          4  


#define  UART_ClearFlags(gstUART)        				do{ memset(gstUART.Flags,0,UART_FLAG_MAX); }while(0)
#define  UART_RxNormalMode_Enter(gstUART)               do{  gstUART.eRxMode=UART_RX_NORMAL_MODE;     }while(0)
#define  UART_RxNormalMode_Exit(gstUART)                do{  gstUART.eRxMode=UART_RX_FRAME_MODE;     }while(0)
#define  UART_RxBufferClear(gstUART)					do{ memset(gstUART.Buffer,0,UART_RX_BUFFER_SIZE);gstUART.unBufferLen=0;}while(0)

//���ڵ���
#ifdef   UART_DEBUG
#define  DBG_Printf  UART1_Printf
#else
#define  DBG_Printf 
#endif


//���ں���
/**
  * @brief  �����ַ���
  * @param  huart: ���ڿ��ƾ��
  * @param  pStr: �ַ���ָ�� 
  */
void SendString(UART_HandleTypeDef *huart,  char *pStr);
void UART_SendByte(UART_HandleTypeDef *huart,  char data);
void print_null(const char *fmt,...);
unsigned char  ReceviceByte(stUART_BaseType *pUartBase,char *pData,unsigned int timeout);


/*********************************************************************************************************************/
#endif

