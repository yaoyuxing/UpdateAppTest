
#include "my_usart.h"
#include "stdarg.h" 
#include "stdio.h"
#include "string.h"
#include "task.h"
/**********************************
配置串口步骤：
1、"usart_config.h"根据宏选择MCU 
2、"usart_config.h"选择需要用到的USART
3、更改宏选择复用GPIO管脚
4、MY_USART_Init 初始化串口
5、若有串口接收中断可选择两种方式 
   a、USART_Interrupt_SoftFlowCtrl 软件流控方式接收帧（桢头、尾各一个字节），外部任务控制 USART1_ISR_REC_TIMEOUT_FLAG 可设置接收超时
	 b、USART_Interrupt_TimerFrameCtrl 时间控制接收帧(外部定时器操作extern变量USART1_ISR_REC_TIMEOUT_FLAGRESET接收结束)
6、选择是否有系统 
"usart_config.h"选配调试及串口打印端口
MY_USART_SendBytes 发送固定长度数据
MY_USART_RecBytes  接收固定长度函数
DEBUG_PRINTF       调试格式化打印
**********************************/
 
 
volatile sUSART_Type gsUSART={0};

#ifdef   USE_FREE_RTOS

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h" 
TimerHandle_t      usart1_rec_timer;   
BaseType_t            usart1_rec_timer_woken=pdFALSE;     
void usart_rec_timer_callback(TimerHandle_t xtimer)
{  
	 USART1_ISR_REC_TIMEOUT_FLAG=SET;
	 USART1_ISR_REC_START_FLAG=RESET; 
	 DEBUG_PRINTF("接收完成\r\n" ); 
   //MY_USART_SendBytes(USART1,(const char *)gsUSART.RecBuf,gsUSART.RecFrameLen); 
}  
#endif


#ifdef MY_STM32F103  

//步骤3 更改宏选择复用GPIO管脚

//USART1
#ifdef USE_USART1 
#define USART1_RX_PIN  GPIO_Pin_10        
#define USART1_TX_PIN  GPIO_Pin_9
#define USART1_PORT    GPIOA
#define USART1_GPIO_RCC   RCC_APB2Periph_GPIOA
#define USART1_TX_PIN_SOURCE GPIO_PinSource9
#define USART1_RX_PIN_SOURCE GPIO_PinSource10
#endif

//USART2
#ifdef USE_USART2
#define USART1_RX_PIN  GPIO_Pin_6        
#define USART1_TX_PIN  GPIO_Pin_5
#define USART1_PORT    GPIOD
#define USART1_GPIO_RCC   RCC_APB2Periph_GPIOD
#define USART1_TX_PIN_SOURCE GPIO_PinSource6
#define USART1_RX_PIN_SOURCE GPIO_PinSource7
#endif

//USART3
#ifdef  USE_USART3 
#define USART1_RX_PIN  GPIO_Pin_11       
#define USART1_TX_PIN  GPIO_Pin_10
#define USART1_PORT    GPIOC
#define USART1_GPIO_RCC   RCC_APB2Periph_GPIOC
#define USART1_TX_PIN_SOURCE GPIO_PinSource6
#define USART1_RX_PIN_SOURCE GPIO_PinSource7
#endif


void MY_F103_USART_Init(eUSART_PORTType eUsart ,eUSART_RecIntType eRecIntSta,eUSART_BaudType eBaud,eUSART_StatType eStat)
{
	USART_InitTypeDef USART_InitStructure;  //串口初始化结构
	GPIO_InitTypeDef GPIO_InitStructure;    //GPIO初始化结构
  NVIC_InitTypeDef NVIC_InitStructure;    //中断配置结构
  USART_TypeDef    *USARTx;
	
  USART_InitStructure.USART_BaudRate =eBaud;            //设计波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;         //默认8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;             //默认停止位1
  USART_InitStructure.USART_Parity = USART_Parity_No;               //默认不校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;      //默认无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;               //默认开TX RX 

	switch(eUsart)                   //配置TX、RX GPIO
	{
		case MY_USART1:
			        USARTx=USART1;                                                      //串口寄存器指针
		          NVIC_InitStructure.NVIC_IRQChannel =USART1_IRQn;                    //配置中断通道    USART1_IRQn
							RCC_APB2PeriphClockCmd(USART1_GPIO_RCC|RCC_APB2Periph_USART1, ENABLE);  //GPIO时钟使能 串口1时钟使能   
							GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		          GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
							GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
							GPIO_Init(USART1_PORT, &GPIO_InitStructure);                        //初始化 IO
		          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		          GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
							GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
							GPIO_Init(USART1_PORT, &GPIO_InitStructure);                        //初始化 IO
		          USART_Init(USARTx, &USART_InitStructure);                           //初始化 USART 
			break;
  	case MY_USART2:
			break;
		case MY_USART3:
			break;
		case MY_USART4: 
 			break;
		default:
			break;
  }
	USART_Cmd(USARTx, ENABLE);             //串口允许
	
	if((eRecIntSta&0xf0)==REC_INT_ENABLE)       //取高四位配置接收中断
	{   
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =eRecIntSta&0x0f;      //取低四位优先级     
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		USART_ITConfig(USARTx,USART_IT_RXNE, ENABLE);	          
		USART_ITConfig(USARTx,USART_IT_TXE, DISABLE);          
	}
}
void MY_F103_USART_SendBytes(USART_TypeDef   *USARTx ,const char * pSendBuf,unsigned int cnt)
{ 
	while(cnt--)
	{
		USART_SendData(USARTx,(uint16_t)(*pSendBuf));                      //发送一个字节
	  while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)       
		{
		}
		pSendBuf++;
	}
}
void MY_F103_USART_RecBytes(USART_TypeDef    *USARTx,char *pRecBuf,unsigned int cnt)
{  
	  while(cnt--)
	  {
			if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET)
			{ 
			  *pRecBuf =USART_ReceiveData(USARTx); 
		 	  pRecBuf++;
				USART_ClearFlag(USARTx,USART_FLAG_RXNE);
			}
	  }
}
#ifdef   USE_USART1               //如果使用USART1编译代码
int  USART1_Interrupt_SoftFlowCtrl(const char FrameHead,const char FrameTail) //串口1软件流控制函数
{
	 char byte;
	 if( USART_GetITStatus(USART1,USART_IT_RXNE)==SET)       //串口1接收寄存器不为空
	 {
		 byte=USART_ReceiveData(USART1);                       //接收数据到byte
		 if(USART1_ISR_REC_TIMEOUT_FLAG==SET)                 //超时充值接收长度及复位接收开始和超时标志
		 { 
			 USART1_ISR_REC_START_FLAG=RESET;                   
			 USART1_ISR_REC_TIMEOUT_FLAG=RESET; 
			 gsUSART.RecFrameLen=0;                             //长度清零
		 }
		 if((byte==FrameHead)&&(USART1_ISR_REC_START_FLAG==RESET))          //判断帧头
		 {
			 USART1_ISR_REC_START_FLAG=SET;                                   //接收开始标志置位
			 gsUSART.RecFrameLen=0;                                           //长度清零
			 gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;                      //接收帧头
		 }	
		 else if((USART1_ISR_REC_START_FLAG==SET)&&(byte!=FrameTail)	)	    //判断帧尾
		 {
			gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;                      //接收帧
		 }			 
		 else  if((USART1_ISR_REC_START_FLAG==SET)&&(byte==FrameTail)	)	     //接收结束
		 {
			 gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;                       //接收帧尾
			 USART1_ISR_REC_START_FLAG=RESET;                                  //复位帧开始标志
			 USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			 USART1_ISR_REC_END_FLAG=SET;                                      //结束标志置位
			 return 1;
		 }
	 } 
	 USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	 return 0;
} 
void  USART1_Interrupt_TimerFrameCtrl(void)        //控制超时帧接收函数
{
	 char byte;
	 if( USART_GetITStatus(USART1,USART_IT_RXNE)==SET)            //接收寄存器不为空标志置位
	 {
		 byte=USART_ReceiveData(USART1);                            //接收数据
		 if(USART1_ISR_REC_START_FLAG==RESET)
		 {
			 	 USART1_ISR_REC_START_FLAG=SET;
			   USART1_ISR_REC_TIMEOUT_FLAG=RESET;
#ifdef USE_FREE_RTOS   
  	 	 xTimerStartFromISR(usart1_rec_timer,(BaseType_t *)&usart1_rec_timer_woken);         //接收开始 接收计时开始  
			 gsUSART.RecFrameLen=0;            //清零接收 
#endif
		 }
		 if((USART1_ISR_REC_TIMEOUT_FLAG!=SET) &&(USART1_ISR_REC_START_FLAG==SET))                      //接收时间未超时
		 {
			  gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;             //接收数据
		 } 
	 }
	 USART_ClearITPendingBit(USART1,USART_IT_RXNE);         //清除标志
}
#endif

#endif


void DEBUG_PRINTF(const char *fmt,...)
{
	#ifdef DEBUG
	  int i = 0;
    va_list args;
    unsigned int n;
    char buffer[DEBUG_FORMAT_BUF];   
    va_start(args,fmt);
    n = vsprintf(buffer,fmt,args); 
    va_end(args);  
    for(i = 0;i < n;i ++)
    { 
       Debug_SendBytes(buffer[i]);
    }
		
	#endif
}





