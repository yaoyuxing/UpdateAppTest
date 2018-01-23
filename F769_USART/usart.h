#ifndef USART_H
#define USART_H

#include "stm32f7xx.h"

#define USART_TEST

#ifdef  USART_TEST
void usart_test_fun(void);
#endif




//USART1
#define USART1_TX_GPIO_CLK_ENABLE()  __GPIOA_CLK_ENABLE()
#define UART1_TX_PIN                 GPIO_PIN_9
#define USART1_TX_GPIO_PORT          GPIOA


#define USART1_RX_GPIO_CLK_ENABLE()  __GPIOA_CLK_ENABLE()
#define UART1_RX_PIN                 GPIO_PIN_10
#define USART1_RX_GPIO_PORT          GPIOA

#define USART1_AF              GPIO_AF7_USART1
#define USART1_CLK_ENABLE()    __USART1_CLK_ENABLE()
#define USART1_IRQ             USART1_IRQn


//USART2
#define USART2_TX_GPIO_CLK_ENABLE()  __GPIOA_CLK_ENABLE()
#define UART2_TX_PIN                 GPIO_PIN_2
#define USART2_TX_GPIO_PORT          GPIOA

#define USART2_RX_GPIO_CLK_ENABLE()  __GPIOA_CLK_ENABLE()
#define UART2_RX_PIN                 GPIO_PIN_3
#define USART2_RX_GPIO_PORT          GPIOA

#define USART2_AF                   GPIO_AF7_USART2
#define USART2_CLK_ENABLE()         __USART2_CLK_ENABLE()
#define USART2_IRQ                  USART2_IRQn













void USART_Init(USART_TypeDef *UARTx,unsigned int Baud);
void UART_SendByte(USART_TypeDef *USARTx,char data);
unsigned char UART_SendBytes(USART_TypeDef *USARTx,char *pBuf,unsigned int unBuffSize);

#endif