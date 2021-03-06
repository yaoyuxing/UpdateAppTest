#include "sys.h"
#include "uart6.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
/*
bound 波特率
*/

UART_HandleTypeDef gUART6_Handler; //UART4句柄
u8 gUSART6_HalRxBuffer[USART6_RXBUFFERSIZE];//HAL库使用的串口接收缓冲
u8 gUSART6_RX[USART6_REC_LEN];
u16 gUART6_NUM;
u8 gUSART6_RX_STATUS;

/*
串口4初始化
*/
void uart6_init(u32 bound)
{
	gUART6_Handler.Instance=USART6;					    //USART6
	gUART6_Handler.Init.BaudRate=bound;				    //波特率
	gUART6_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	gUART6_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	gUART6_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	gUART6_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	gUART6_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&gUART6_Handler);					    //HAL_UART_Init()会使能UART4

	HAL_UART_Receive_IT(&gUART6_Handler, (u8 *)gUSART6_HalRxBuffer, USART6_RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量

}


//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄
#define  UART6_PIN_TX	GPIO_PIN_6 //C6
#define  UART6_PIN_RX	GPIO_PIN_7//C7

void UART6_HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART6)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART6_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin=UART6_PIN_TX;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF8_USART6;	//复用为UART4
		
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin=UART6_PIN_RX;			//PA10
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//初始化PA10
		
#if EN_UART6_RX
		HAL_NVIC_EnableIRQ(USART6_IRQn);		//使能USART1中断通道
		HAL_NVIC_SetPriority(USART6_IRQn,2,1);	//抢占优先级3，子优先级3
#endif	
	}

}


//串口6中断服务程序
void USART6_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&gUART6_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&gUART6_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&gUART6_Handler, (u8 *)gUSART6_HalRxBuffer, USART6_RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>maxDelay) break;	
	}
} 
/*
串口4发送一个byte数据
*/
u8 UART6_SendByte(u8 ch)
{ 	
	while((USART6->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART6->TDR = ch;      
	return ch;
}
/*
串口4发送一个字符串
*/
 void UART6_SendBytes(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=strlen((const char*)p_comm);
	for(num=0;num<len;num++)
	{
		UART6_SendByte(*(p_comm+num));
		delay_xms(1);
	}
}
/*
发送指定长度的数据
*/
 void UART6_SendNumBytes(char *data,unsigned int len)
{
	char *p_comm=data;
	uint16_t num=0;
	for(num=0;num<len;num++)
	{
		UART6_SendByte(*(p_comm+num));
	}
}
/*
串口接收数据处理回调函数
*/

void UART6_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART6)//如果是串口4
	{
		gUSART6_RX[gUART6_NUM++]=gUSART6_HalRxBuffer[0];

		if(gUART6_NUM>=USART6_REC_LEN)
		{
			gUSART6_RX_STATUS|=0x1;
			gUART6_NUM=0;
		}
	
	}
}




