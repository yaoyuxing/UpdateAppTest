#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "stdlib.h"
#include <stdarg.h>



stUART_RxCtrlType  gstRxCtrl;
#ifdef USE_USART1 
stUART_BaseType   gstUart1;
#endif
#ifdef USE_USART2 
stUART_BaseType   gstUart2;
#endif
#ifdef USE_USART3 
stUART_BaseType   gstUart3;
#endif
#ifdef USE_UART4 
stUART_BaseType   gstUart4;
#endif
#ifdef USE_UART5 
stUART_BaseType   gstUart5;
#endif
#ifdef USE_USART6 
stUART_BaseType   gstUart6;
#endif
#ifdef USE_UART7 
stUART_BaseType   gstUart7;
#endif


#ifdef  USART_TEST

#define gLocalUartHandle   gUART2_Handle  
#define gLocalUartSendBytes        COM2_SendBytes 
#define gLocalUartRecBytes(pdata,size)             do{COM2_RecBytes(pdata,size) ; while(HAL_UART_GetState((UART_HandleTypeDef*)&gLocalUartHandle)!=HAL_UART_STATE_READY ){}}while(0)
void UpdateUartInit(unsigned int Baud )
{ 
	gLocalUartHandle.Instance= USART2;
	gLocalUartHandle.Init.BaudRate=Baud;
	gLocalUartHandle.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	gLocalUartHandle.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	gLocalUartHandle.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位 
	gLocalUartHandle.Init.Mode=UART_MODE_TX_RX;		    //
	HAL_UART_Init( (UART_HandleTypeDef*)&gLocalUartHandle); 
	COM2_RecBytes( (uint8_t *)&(gstUart2.recByte),1); 
}
void usart_test_fun(void)
{ 
	uint8_t pData[100]={0};
	UpdateUartInit( 115200);  
 // gLocalUartSendBytes( (uint8_t *)"123456",6);  
//	gLocalUartRecBytes(pData,10);
//  gLocalUartSendBytes( (uint8_t *)pData,6);  
	while(1)
	{  
	}
}
#endif 
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	unsigned int Uartx=(unsigned int )huart->Instance;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_InitStruct.Pull=GPIO_PULLUP;			//上拉
	GPIO_InitStruct.Speed=GPIO_SPEED_FAST;		//高速
	switch(Uartx)
	{
		#ifdef USE_USART1
		case USART1_BASE:
			USART1_CLK_ENABLE();
			USART1_TX_CLK_ENBALE();
			USART1_RX_CLK_ENABLE();
			GPIO_InitStruct.Pin=USART1_TX_PIN;
			GPIO_InitStruct.Alternate=USART1_TX_AF;
			HAL_GPIO_Init(USART1_TX_PORT,&GPIO_InitStruct);	
			GPIO_InitStruct.Pin=USART1_RX_PIN;
			GPIO_InitStruct.Alternate=USART1_RX_AF;
			HAL_GPIO_Init(USART1_RX_PORT,&GPIO_InitStruct);	   		
			break;
		#endif
		#ifdef USE_USART2
		case USART2_BASE:
			USART2_CLK_ENABLE();
			USART2_TX_CLK_ENBALE();
			USART2_RX_CLK_ENABLE();
			GPIO_InitStruct.Pin=USART2_TX_PIN;
			GPIO_InitStruct.Alternate=USART2_TX_AF;
			HAL_GPIO_Init(USART2_TX_PORT,&GPIO_InitStruct);	
			GPIO_InitStruct.Pin=USART2_RX_PIN;
			GPIO_InitStruct.Alternate=USART2_RX_AF;
			HAL_GPIO_Init(USART2_RX_PORT,&GPIO_InitStruct);	   		
			break;
		#endif
		#ifdef USE_USART3
		case USART3_BASE:
			USART3_CLK_ENABLE();
			USART3_TX_CLK_ENBALE();
			USART3_RX_CLK_ENABLE();
			GPIO_InitStruct.Pin=USART3_TX_PIN;
			GPIO_InitStruct.Alternate=USART3_TX_AF;
			HAL_GPIO_Init(USART3_TX_PORT,&GPIO_InitStruct);	
			GPIO_InitStruct.Pin=USART3_RX_PIN;
			GPIO_InitStruct.Alternate=USART3_RX_AF;
			HAL_GPIO_Init(USART3_RX_PORT,&GPIO_InitStruct);	  
			HAL_NVIC_SetPriority(USART3_IRQn, 7, 0);
			HAL_NVIC_EnableIRQ(USART3_IRQn);		
			break;
		#endif
		#ifdef USE_UART5
		case UART5_BASE: 
			UART5_TX_CLK_ENBALE();
			UART5_RX_CLK_ENABLE();
			UART5_CLK_ENABLE();
			GPIO_InitStruct.Pin=UART5_TX_PIN;
			GPIO_InitStruct.Alternate=UART5_TX_AF;
			HAL_GPIO_Init(UART5_TX_PORT,&GPIO_InitStruct);	
			GPIO_InitStruct.Pin=UART5_RX_PIN;
			GPIO_InitStruct.Alternate=UART5_RX_AF;
			HAL_GPIO_Init(UART5_RX_PORT,&GPIO_InitStruct);	
			HAL_NVIC_SetPriority(UART5_IRQn, 6, 0);
			HAL_NVIC_EnableIRQ(UART5_IRQn);	
			break;
		#endif
	} 
} 
static int UART_ReceiveCallback(stUART_RxCtrlType *pstRxCtrl)
{  
	char Byte=*pstRxCtrl->pReByte;
	if(pstRxCtrl->ucUseEscape)
	{
		if((Byte==ESCAPE_CHAR)&&(pstRxCtrl->ucEscFlag==RESET))//接到转义字符
		{
			pstRxCtrl->ucEscFlag=SET;  //设置标记
			return 0;
		}
		else if(pstRxCtrl->ucEscFlag==SET) 
		{ 
			Byte=Byte^XOR_CHAR; //  取转义后字符
			pstRxCtrl->ucEscFlag=RESET; 
		}  
	}
	if(*pstRxCtrl->pMode==UART_RX_FRAME_MODE)
	{
		if(Byte==pstRxCtrl->cHead)//找到帧头
		{ 
			*pstRxCtrl->pBufferLen=0;
			*(pstRxCtrl->pBuffer)=pstRxCtrl->cHead;  //取帧头
			(*pstRxCtrl->pBufferLen)+=1;       //长度加1
			*pstRxCtrl->pFrameStartFlag=SET; //帧开始
		}
		else if((Byte!=pstRxCtrl->cTail)&&((*pstRxCtrl->pFrameStartFlag)==SET)) //帧开始接收数据
		{
			pstRxCtrl->pBuffer[*pstRxCtrl->pBufferLen]=Byte;
			(*pstRxCtrl->pBufferLen)+=1;
		}
		else if(((*pstRxCtrl->pFrameStartFlag)==SET)&&(Byte==pstRxCtrl->cTail)) //接收到帧尾
		{
			pstRxCtrl->pBuffer[*pstRxCtrl->pBufferLen]=Byte;//取帧尾
			(*pstRxCtrl->pBufferLen)+=1;
			*(pstRxCtrl->pRecFrameFlag)=SET;  
		}
	}
	else if(*pstRxCtrl->pMode==UART_RX_LEN_MODE)       //接收固定长度数据
	{
		if(*gstRxCtrl.pRxSetLen)    //长度大于0 
		{
			if((*pstRxCtrl->pFrameStartFlag)==RESET)  //帧开始
			{
				*(pstRxCtrl->pRecFrameFlag)=RESET;   
				*pstRxCtrl->pBufferLen=0;
				pstRxCtrl->pBuffer[*pstRxCtrl->pBufferLen]=Byte;
				(*pstRxCtrl->pBufferLen)+=1;
				*(pstRxCtrl->pFrameStartFlag)=SET;
			}
			else if(((*pstRxCtrl->pFrameStartFlag)==SET)&&(*pstRxCtrl->pBufferLen<(*gstRxCtrl.pRxSetLen)))//接收数据直到超过长度
			{
				pstRxCtrl->pBuffer[*pstRxCtrl->pBufferLen]=Byte;
				(*pstRxCtrl->pBufferLen)+=1;
			}
			else
			{
				*(pstRxCtrl->pRecFrameFlag)=RESET;   
				pstRxCtrl->pBuffer[*pstRxCtrl->pBufferLen]=Byte;
				(*pstRxCtrl->pBufferLen)+=1;
				*(pstRxCtrl->pRecFrameFlag)=SET;   
			}
		} 
	}
	else if(*pstRxCtrl->pMode==UART_RX_NORMAL_MODE)       //常规模式取数据
	{
		if(*pstRxCtrl->pBufferLen<UART_RX_BUFFER_SIZE)    //没超过缓冲区
		{
			pstRxCtrl->pBuffer[*pstRxCtrl->pBufferLen]=Byte;
			(*pstRxCtrl->pBufferLen)+=1;
		}
		else
		{
			*pstRxCtrl->pBufferLen=0; //清零重新开始
		}
	}
	return 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  //串口接收中断回调函数
{   
	gstRxCtrl.Uartx=(unsigned int )huart->Instance;
	switch(gstRxCtrl.Uartx)
	{ 
		#ifdef USE_USART1
		case USART1_BASE :
			gstRxCtrl.pReByte=&gstUart1.ucRecByte; 
			gstRxCtrl.pBuffer=gstUart1.Buffer;
			gstRxCtrl.pBufferLen=&gstUart1.unBufferLen;
			gstRxCtrl.pRecFrameFlag=(unsigned char*)&UART1_RX_GOT_FRAME_FLAG; 
			gstRxCtrl.pRecByteFlag=(unsigned char*)&UART1_RX_GOT_BYTE_FLAG;
			gstRxCtrl.pFrameStartFlag=(unsigned char*)&UART1_RX_FRAME_START_FLAG;
			gstRxCtrl.pMode=&gstUart1.eRxMode;  
			gstRxCtrl.cHead=UART1_HEAD_CHAR;
			gstRxCtrl.cTail=UART1_TAIL_CHAR;
			gstRxCtrl.ucUseEscape=SET;
			gstRxCtrl.pRxSetLen=&gstUart1.unRxSetLen;
			break;
		#endif 
		#ifdef USE_USART2
		case USART2_BASE :
			gstRxCtrl.pReByte=&gstUart2.ucRecByte; 
			gstRxCtrl.pBuffer=gstUart2.Buffer;
			gstRxCtrl.pBufferLen=&gstUart2.unBufferLen;
			gstRxCtrl.pRecFrameFlag=(unsigned char*)&UART2_RX_GOT_FRAME_FLAG; 
			gstRxCtrl.pRecByteFlag=(unsigned char*)&UART2_RX_GOT_BYTE_FLAG;
			gstRxCtrl.pFrameStartFlag=(unsigned char*)&UART2_RX_FRAME_START_FLAG;
			gstRxCtrl.pMode=&gstUart2.eRxMode;  
			gstRxCtrl.cHead=UART2_HEAD_CHAR;
			gstRxCtrl.cTail=UART2_TAIL_CHAR;
			gstRxCtrl.ucUseEscape=SET;
			gstRxCtrl.pRxSetLen=&gstUart2.unRxSetLen;
			break;
		#endif 
		#ifdef USE_UART5
		case UART5_BASE :
			gstRxCtrl.pReByte=&gstUart5.ucRecByte; 
			gstRxCtrl.pBuffer=gstUart5.Buffer;
			gstRxCtrl.pBufferLen=&gstUart5.unBufferLen;
			gstRxCtrl.pRecFrameFlag=(unsigned char*)&UART5_RX_GOT_FRAME_FLAG; 
			gstRxCtrl.pRecByteFlag=(unsigned char*)&UART5_RX_GOT_BYTE_FLAG;
			gstRxCtrl.pFrameStartFlag=(unsigned char*)&UART5_RX_FRAME_START_FLAG;
			gstRxCtrl.pMode=&gstUart5.eRxMode;  
			gstRxCtrl.cHead=UART5_HEAD_CHAR;
			gstRxCtrl.cTail=UART5_TAIL_CHAR;
			gstRxCtrl.ucUseEscape=SET;
			gstRxCtrl.pRxSetLen=&gstUart5.unRxSetLen;
			break;
		#endif 
	} 
	UART_ReceiveCallback(&gstRxCtrl);
}
/**
  * @brief  发送字符串
  * @param  huart: 串口控制句柄
  * @param  pStr: 字符串指针 
  */
void SendString(UART_HandleTypeDef *huart,  char *pStr)
{
	unsigned int Uartx=(unsigned int )huart->Instance;
	while(*pStr!='\0')
	{
        switch(Uartx)
		{ 
			#ifdef USE_USART1
			case USART1_BASE:  
				COM1_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
			#ifdef USE_USART2
			case USART2_BASE:  
				COM2_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
			#ifdef USE_USART3
			case USART3_BASE:  
				COM3_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
			#ifdef USE_UART4
			case UART4_BASE:  
				COM4_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
			#ifdef USE_UART5
			case UART5_BASE:  
				COM5_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
			#ifdef USE_USART6
			case USART6_BASE:  
				COM6_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
			#ifdef USE_UART7
			case UART7_BASE:  
				COM7_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
			#ifdef USE_UART8
			case UART8_BASE:  
				COM8_SendBytes( (unsigned char *)pStr,1); 
				break;
			#endif
		}
		pStr++;
	}  
} 
#ifdef USE_USART1
/**
  * @brief  串口格式输出
  * @param  fmt:  格式
  */
void UART1_Printf(const char *fmt,...)
{
	int i = 0;
	va_list args;
	unsigned int n;
	char buffer[1024]; 
	va_start(args,fmt);
	n = vsprintf(buffer,fmt,args);
	va_end(args); 
	for(i = 0;i < n;i ++)
	{
		COM1_SendBytes( (unsigned char *)&buffer[i],1);
	}
} 
#endif
unsigned char  ReceviceByte(stUART_BaseType *pUartBase,char *pData,unsigned int timeout)
{    
	while(timeout--)
	{
		if(pUartBase->Flags[UART_RX_BYTE_FLAG_POS]==SET)
		{
			pUartBase->Flags[UART_RX_BYTE_FLAG_POS]=RESET;
			*pData=pUartBase->ucRecByte;
			return 0;
		} 
	}
	return 1;
}

//void  ReceviceBytes(char *pData,unsigned int len)
//{  
//	while(len)
//	{
//		if(  ReceviceByte(pData)==0)
//		len--;
//	}
//} 
void print_null(const char *fmt,...)
{
	
}
