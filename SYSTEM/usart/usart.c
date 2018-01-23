#include "usart.h" 
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //os ʹ��	  
#endif
 	  
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
//	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
//	USART1->TDR = (u8) ch;    
	
	ITM_SendChar(ch);
	return ch;
}


u8 uarst1send(u8 ch)
{ 	
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR = (u8) ch;      
	return ch;
}
void _ttywrch(int ch){};
#endif 
/*
����1����ض���
*/
//���Դ���
UART_HandleTypeDef UART1_Handler; //UART1���
//����1�жϷ������
//ע��,��ȡUSARTx->ISR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���	
u8 USART1_HalRxBuffer[USART1_RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���

/*
����1����ض��� end
*/

 
//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound)
{	
	/*UART1 ��ʼ������ ��Ϊ���Խӿ�*/
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=57600;		 		    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1

	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)USART1_HalRxBuffer, USART1_RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
 

}

 

 

 
//����1�жϷ������
void USART1_IRQHandler(void)                	
{  
	HAL_UART_IRQHandler(&UART1_Handler);	//����HAL���жϴ����ú��� 
} 

 




