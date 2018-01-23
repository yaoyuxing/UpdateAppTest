#include "usart.h" 
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //os 使用	  
#endif
 	  
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
//	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
//	USART1->TDR = (u8) ch;    
	
	ITM_SendChar(ch);
	return ch;
}


u8 uarst1send(u8 ch)
{ 	
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR = (u8) ch;      
	return ch;
}
void _ttywrch(int ch){};
#endif 
/*
串口1的相关定义
*/
//调试串口
UART_HandleTypeDef UART1_Handler; //UART1句柄
//串口1中断服务程序
//注意,读取USARTx->ISR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	
u8 USART1_HalRxBuffer[USART1_RXBUFFERSIZE];//HAL库使用的串口接收缓冲

/*
串口1的相关定义 end
*/

 
//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	/*UART1 初始化设置 作为调试接口*/
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=57600;		 		    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1

	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)USART1_HalRxBuffer, USART1_RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
 

}

 

 

 
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{  
	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数 
} 

 




