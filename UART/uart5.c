#include "sys.h"
#include "uart5.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
#include "debug.h"
/*
bound 波特率
*/

UART_HandleTypeDef gUART5_Handler; //UART5句柄
u8 gUSART5_HalRxBuffer[USART5_RXBUFFERSIZE];//HAL库使用的串口接收缓冲
u8 gUSART5_RX[USART5_REC_LEN];
u16 gUART5_NUM;
u8 gUSART5_RX_STATUS;

/*
串口4初始化
*/
void uart5_init(u32 bound)
{
	gUART5_Handler.Instance=UART5;					    //USART5
	gUART5_Handler.Init.BaudRate=bound;				    //波特率
	gUART5_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	gUART5_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	gUART5_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	gUART5_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	gUART5_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&gUART5_Handler);					    //HAL_UART_Init()会使能UART5

	HAL_UART_Receive_IT(&gUART5_Handler, (u8 *)gUSART5_HalRxBuffer, USART5_RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量

}
#if USART5_DEVELOPMENT_BOARD

	//UART底层初始化，时钟使能，引脚配置，中断配置
	//此函数会被HAL_UART_Init()调用
	//huart:串口句柄
	#define  UART5_PIN_TX	GPIO_PIN_12
	#define  UART5_PIN_RX	GPIO_PIN_2

	void UART5_HAL_UART_MspInit(UART_HandleTypeDef *huart)
	{
		//GPIO端口设置
		GPIO_InitTypeDef GPIO_Initure;
		
		if(huart->Instance==UART5)//如果是串口1，进行串口1 MSP初始化
		{
			__HAL_RCC_GPIOC_CLK_ENABLE();			//使能GPIOA时钟
			__HAL_RCC_GPIOD_CLK_ENABLE();			//使能GPIOA时钟
			__HAL_RCC_UART5_CLK_ENABLE();			//使能USART1时钟
		
			GPIO_Initure.Pin=UART5_PIN_TX;			//PC12
			GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
			GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
			GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
			GPIO_Initure.Alternate=GPIO_AF8_UART5;	//复用为UART5
			
			HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//初始化PA9

			GPIO_Initure.Pin=UART5_PIN_RX;			//PD2
			HAL_GPIO_Init(GPIOD,&GPIO_Initure);	   	//初始化PA10
			
		#if EN_UART5_RX
			HAL_NVIC_EnableIRQ(UART5_IRQn);		//使能USART1中断通道
			HAL_NVIC_SetPriority(UART5_IRQn,2,1);	//抢占优先级3，子优先级3
		#endif	
		}

	}
#else 
	//UART底层初始化，时钟使能，引脚配置，中断配置
	//此函数会被HAL_UART_Init()调用
	//huart:串口句柄
	#define  UART5_PIN_TX	GPIO_PIN_13
	#define  UART5_PIN_RX	GPIO_PIN_12

	void UART5_HAL_UART_MspInit(UART_HandleTypeDef *huart)
	{
		//GPIO端口设置
		GPIO_InitTypeDef GPIO_Initure;
		
		if(huart->Instance==UART5)//如果是串口5，进行串口5 MSP初始化
		{
			__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
			__HAL_RCC_UART5_CLK_ENABLE();			//使能USART1时钟
		
			GPIO_Initure.Pin=UART5_PIN_TX|UART5_PIN_RX;			
			GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
			GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
			GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
			GPIO_Initure.Alternate=GPIO_AF8_UART5;	//复用为UART5
			HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化P
			
		#if EN_UART5_RX
			HAL_NVIC_EnableIRQ(UART5_IRQn);		//使能USART1中断通道
			HAL_NVIC_SetPriority(UART5_IRQn,2,1);	//抢占优先级3，子优先级3
		#endif	
		}

	}
#endif

//串口5中断服务程序
void UART5_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&gUART5_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&gUART5_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&gUART5_Handler, (u8 *)gUSART5_HalRxBuffer, USART5_RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>maxDelay) break;	
	}
} 
/*
串口5发送一个byte数据
*/
u8 UART5_SendByte(u8 ch)
{ 	
	while((UART5->ISR&0X40)==0);//循环发送,直到发送完毕   
	UART5->TDR = ch;      
	return ch;
}
/*
串口5发送一个字符串
*/
void UART5_SendBytes(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=strlen((const char*)p_comm);
	for(num=0;num<len;num++)
	{
		UART5_SendByte(*(p_comm+num));
	}
}
/*
串口5发送一段内存的数据
*/
void UART5_SendBytesSizeof(char *data)
{
	int num=0;
	unsigned int len=0;
	char *p_data=data;
	len=sizeof((const char*)p_data);
	for(num=0;num<len;num++)
	{
		UART5_SendByte(*(p_data+num));
		delay_xms(1);
	}
}


/*
发送指定长度的数据
*/
void UART5_SendDataBytes(char *data,u16 len)
{
	int num=0;
	for(num=0;num<len;num++)
	{
		UART5_SendByte(*(data+num));
#if 0
	printf("%c",*(data+num));
#endif

	}
}

/*
串口接收数据处理回调函数
*/
#include "bluetooth.h"
#include "rtk.h"
extern char gTestFlag;
void UART5_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static char escape_falg=0,num=0;
	
	if(huart->Instance==UART5)//如果是串口4
	{
#if 0
		if(gTestFlag==1)
		{
			if(ESCAPE_CHARACTER==gUSART5_HalRxBuffer[0])
			{
				escape_falg = 1;
			}else if(escape_falg != 1)
			{
				rtkSendByte(gUSART5_HalRxBuffer[0]);
			}
			if(escape_falg == 1)
			{
				num+=1;
				if(num==2)
				{
					num=0;
					escape_falg=0;
					gUSART5_HalRxBuffer[0]=XOR_VALUE^gUSART5_HalRxBuffer[0];
					rtkSendByte(gUSART5_HalRxBuffer[0]);
				}
			}
			
			
//			rtkSendByte(gUSART5_HalRxBuffer[0]);
//			uarst1send(gUSART5_HalRxBuffer[0]);
			//printf("%c",gUSART5_HalRxBuffer[0]);
		}else{
			gUSART5_RX[gUART5_NUM++]=gUSART5_HalRxBuffer[0];
		}
		if(gUART5_NUM>=USART5_REC_LEN)
		{
			gUSART5_RX_STATUS|=0x1;
			gUART5_NUM=0;
		}
#endif
#if 1
		if(gBlRtcmOpenFlag==1&&gUSART5_HalRxBuffer[0]!='#')//接收的是rtcm数据
		{
			if(ESCAPE_CHARACTER==gUSART5_HalRxBuffer[0])
			{
				escape_falg = 1;
			}else if(escape_falg != 1)
			{
				
				//rtkSendByte(gUSART5_HalRxBuffer[0]);
				RtkSendRtcmUartByte(gUSART5_HalRxBuffer[0]);
			}
			if(escape_falg==1)
			{
				num+=1;
				if(num==2)
				{
					num=0;
					escape_falg=0;
					rtkSendByte(XOR_VALUE^gUSART5_HalRxBuffer[0]);
				}
			}
			
		}else {
			
			gUSART5_RX[gUART5_NUM++]=gUSART5_HalRxBuffer[0];
			gBlRtcmOpenFlag=0;
		}
		
//		gUSART5_RX[gUART5_NUM++]=gUSART5_HalRxBuffer[0];
#if DEBUG_UART5
		//uarst1send(gUSART5_HalRxBuffer[0]);
		printf("%x",gUSART5_HalRxBuffer[0]);
#endif
		if(gUART5_NUM>=USART5_REC_LEN)
		{
			gUSART5_RX_STATUS|=0x1;
			gUART5_NUM=0;
		}
#endif
	
	}
}




