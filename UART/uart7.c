/*
电台数据收发串口
*/
#include "sys.h"
#include "uart7.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
#include "debug.h"
/*
bound 波特率
*/

UART_HandleTypeDef gUART7_Handler; //UART7句柄
u8 	gUSART7_HalRxBuffer[USART7_RXBUFFERSIZE];//HAL库使用的串口接收缓冲
u8 	gUSART7_RX[USART7_REC_LEN];
u16 gUART7_NUM;
u8 	gUSART7_RX_STATUS;

/*
串口7初始化
*/
void UART7_init(u32 bound)
{
	gUART7_Handler.Instance=UART7;					    //USART5
	gUART7_Handler.Init.BaudRate=bound;				    //波特率
	gUART7_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	gUART7_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	gUART7_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	gUART7_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	gUART7_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&gUART7_Handler);					    //HAL_UART_Init()会使能UART5

	HAL_UART_Receive_IT(&gUART7_Handler, (u8 *)gUSART7_HalRxBuffer, USART7_RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量

}
/*
修改波特率函数
*/
void UART7_Bound(u32 bound)
{
	gUART7_Handler.Init.BaudRate=bound;				    //波特率
	HAL_UART_Init(&gUART7_Handler);					    //HAL_UART_Init()会使能UART5
}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄
#define  UART7_PIN_TX	GPIO_PIN_7
#define  UART7_PIN_RX	GPIO_PIN_6
#define  UART7_GPIO_TX	GPIOF
#define  UART7_GPIO_RX	GPIOF

void UART7_HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==UART7)//如果是串口7，进行串口7 MSP初始化
	{
		__HAL_RCC_GPIOF_CLK_ENABLE();			//使能GPIOF时钟
		__HAL_RCC_UART7_CLK_ENABLE();			//使能USART7时钟
	
		GPIO_Initure.Pin=UART7_PIN_TX;			
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF8_UART7;	//复用为UART7
		
		HAL_GPIO_Init(UART7_GPIO_TX,&GPIO_Initure);	   	

		GPIO_Initure.Pin=UART7_PIN_RX;			
		HAL_GPIO_Init(UART7_GPIO_RX,&GPIO_Initure);	   	
		
#if EN_UART7_RX
		HAL_NVIC_EnableIRQ(UART7_IRQn);		//使能USART1中断通道
		HAL_NVIC_SetPriority(UART7_IRQn,2,1);	//抢占优先级3，子优先级3
#endif	
	}

}


//串口7中断服务程序
void UART7_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&gUART7_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&gUART7_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&gUART7_Handler, (u8 *)gUSART7_HalRxBuffer, USART7_RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>maxDelay) break;	
	}
} 
/*
串口7发送一个byte数据
*/
u8 UART7_SendByte(u8 ch)
{ 	
	while((UART5->ISR&0X40)==0);//循环发送,直到发送完毕   
	UART7->TDR = ch;      
	return ch;
}
/*
串口7发送一个字符串
*/
void UART7_SendBytesLen(char *data ,uint16_t len)
{
	int num=0;
	for(num=0;num<len;num++)
	{
		UART7_SendByte(*(data+num));
		delay_ms(2);
	}
}
/*
串口7发送一个字符串
*/
void UART7_SendBytes(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=strlen((const char*)p_comm);
	for(num=0;num<len;num++)
	{
		UART7_SendByte(*(p_comm+num));
		delay_ms(1);
		
	}
}
/*
串口7发送一段内存的数据
*/
void UART7_SendBytesSizeof(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=sizeof((const char*)p_comm);
	for(num=0;num<len;num++)
	{
		UART7_SendByte(*(p_comm+num));
	}
}

/*
串口接收数据处理回调函数
*/

void UART7_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==UART7)//如果是串口7
	{
		gUSART7_RX[gUART7_NUM++]=gUSART7_HalRxBuffer[0];
#if DEBUG_UART7
		printf("%c ",gUSART7_HalRxBuffer[0]);
#endif
		if(gUART7_NUM>=USART7_REC_LEN)
		{
			gUSART7_RX_STATUS|=0x1;
			gUART7_NUM=0;
		}
	
	}
}

void UART7_Test(char* data)
{
	while(1)
	{
		UART7_SendBytes(data);
		delay_ms(100);
	}
	
}


