#include "usart.h"
#include "stm32f7xx_hal.h"


#define   UART_MAX         7
volatile unsigned char gcSendLock[UART_MAX]={0};







#define  __UART_SEND_LOCK(x)        do{ gcSendLock[(x)-1] =1;}while(0)
#define  __UART_SEND_UNLOCK(x)      do{ gcSendLock[(x)-1] =0;}while(0)
#define  UART_SendStatus(x)             gcSendLock[(x)-1]

static unsigned char GetUART_Num(USART_TypeDef *UARTx);


 
enum usartstatus
{
	USART_BUSY=1,
	USART_READY=0
};


void usart_test_fun(void)
{
	USART_Init(USART2,9600);
	UART_SendByte(USART2,'A' );
	while(1);
}

static void USART_GPIO_Init(USART_TypeDef *UARTx)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
	unsigned char ucUart=GetUART_Num(UARTx);
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST; 
	
  switch(ucUart)
	{
		case 1:
	    USART1_TX_GPIO_CLK_ENABLE();
			USART1_RX_GPIO_CLK_ENABLE();
		  USART1_CLK_ENABLE();
			RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
			RCC_PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
			HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit); 
			GPIO_InitStruct.Pin=UART1_TX_PIN;  
			GPIO_InitStruct.Alternate = USART1_AF;
		  HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);
		  GPIO_InitStruct.Pin=UART1_RX_PIN; 
			HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct); 
		  break;
		case 2:
	    USART2_TX_GPIO_CLK_ENABLE();
			USART2_RX_GPIO_CLK_ENABLE();
		  USART2_CLK_ENABLE();
			RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
			RCC_PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
			HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit); 
			GPIO_InitStruct.Pin=UART2_TX_PIN;  
			GPIO_InitStruct.Alternate = USART2_AF;
		  HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);
		  GPIO_InitStruct.Pin=UART2_RX_PIN; 
			HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct); 
	 //   HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART2中断通道
	 // 	HAL_NVIC_SetPriority(USART2_IRQn,3,2);			//抢占优先级3，子优先级3
		  break;
//		case 3:
//			GPIO_InitStruct.Pin=UART3_TX_PIN|UART3_RX_PIN;  
//			GPIO_InitStruct.Alternate = USART3_AF;
//		  break;
//		case 4:
//			GPIO_InitStruct.Pin=UART4_TX_PIN|UART4_RX_PIN;  
//			GPIO_InitStruct.Alternate = USART4_AF;
//		  break;
//		case 5:
//			GPIO_InitStruct.Pin=UART5_TX_PIN|UART5_RX_PIN;  
//			GPIO_InitStruct.Alternate = USART5_AF;
//		  break;
//		case 6:
//			GPIO_InitStruct.Pin=UART6_TX_PIN|UART6_RX_PIN;  
//			GPIO_InitStruct.Alternate = USART6_AF;
//		  break;
//		case 7:
//			GPIO_InitStruct.Pin=UART7_TX_PIN|UART7_RX_PIN;  
//			GPIO_InitStruct.Alternate = USART7_AF;
//		  break;
	}
	
	
}
void USART_Init(USART_TypeDef *UARTx,unsigned int Baud)
{
	UART_HandleTypeDef UartHandle;
	UartHandle.Instance        = UARTx; 
  UartHandle.Init.BaudRate   = Baud;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;   
  USART_GPIO_Init(UARTx);
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    while(1){}//error code 
  } 
}
static unsigned char GetUART_Num(USART_TypeDef *UARTx)
{
	unsigned int usart=(unsigned int )UARTx;
	switch(usart)
	{
		case (unsigned int)USART1:
			return 1;
		case (unsigned int)USART2:
			return 2;
		case (unsigned int)USART3:
			return 3;
		case (unsigned int)UART4:
			return 4;
		case (unsigned int)UART5:
			return 5;
		case (unsigned int)USART6:
			return 6;
		case (unsigned int)UART7:
			return 7; 
		default:
			while(1){}
	}
}
void UART_SendByte(USART_TypeDef *USARTx,char data)
{ 
	 while((USARTx->ISR&0X40)==0);//发送一个字节
	 USARTx->TDR = data; 
}
unsigned char UART_SendBytes(USART_TypeDef *USARTx,char *pBuf,unsigned int unBuffSize)
{
	 unsigned char num=GetUART_Num(USARTx);
	if(UART_SendStatus(num)==USART_BUSY)
	{
		return USART_BUSY;
	}
	 __UART_SEND_LOCK(num);
  while(unBuffSize--)
  {
		UART_SendByte(USARTx,*pBuf);
		pBuf++;
	}
	 __UART_SEND_UNLOCK(num);
}





