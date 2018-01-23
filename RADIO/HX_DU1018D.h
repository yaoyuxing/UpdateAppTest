#ifndef __HX_DU1018D_H_
#define __HX_DU1018D_H_
#include "uart7.h"
#include "sys.h"
#include "HX_DU1018D_AT.h"
#define	Du1018d_RX_BUF_LEN		USART7_REC_LEN
#define	Du1018d_RX_BUF 			gUSART7_RX
#define	Du1018d_RX_BUF_NUM 		gUART7_NUM
#define	Du1018d_RX_STATUS		gUSART7_RX_STATUS

#define	Du1018dSendByteS(X)				UART7_SendBytes(X)	//发送多个字节的数据  以'\0'结束
#define	Du1018dSendSizeof(X) 			UART7_SendBytesSizeof(X)//发送一段内存
#define	Du1018dSendByte(X)				UART7_SendByte(X)	//发送一个字节
#define	Du1018dSendByteSLen(data ,len )	UART7_SendBytesLen(data ,len)	//发送一个字节
#define	Du1018dUartBound(X)				UART7_Bound(X)		//设置电台对应的串口的波特率
/*

*/


/*
DU1018D模式切换引脚定义
*/
#define	DU1018D_MODE_PIN		GPIO_PIN_3
#define	DU1018D_MODE_GPIO		GPIOC
#define DU1018D_MODE(n)  		(n?HAL_GPIO_WritePin(DU1018D_MODE_GPIO,DU1018D_MODE_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(DU1018D_MODE_GPIO,DU1018D_MODE_PIN,GPIO_PIN_RESET))
						  
typedef enum  eDU1018D_MODE
{
	eDU1018D_MODE_Configuration=0 , //配置
	eDU1018D_MODE_Ordinary			, //普通
	
}eDU1018D_MODE;						  


typedef  struct 
{
	uint32_t CommandSend:1;//命令是否发送成功 
	uint32_t Channel:6;//0-63
	uint32_t	:32-7;
}stDu1018dStatusBit;
typedef union{
	uint32_t				data;
	stDu1018dStatusBit	bit;
}stDu1018dStatus;
//CommandSend
typedef enum  eDU1018D_CommandSend
{
	eDU1018D_CommandSendY = 0, //成功 
	eDU1018D_CommandSendN, 
}eDU1018D_CommandSend;		
/*
发送命令：发送一条命令等待回显发送成功如果不成功重发
command:拼接的命令
parameter：命令的参数
parameterY_N：有没有参数的标记
Du1018dStatus:标记命令是否发送成功
*/
extern void Du1018dSendCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,stDu1018dStatus *Du1018dStatus);

/*
设置通道：
*/
extern void Du1018dSetChannel(u8 *channel,u8 data_len,stDu1018dStatus *Du1018dStatus);
/*
读通道：
*/
extern void Du1018dReadChannel(u8 *channel,stDu1018dStatus *Du1018dStatus);
extern void Du1018d_Test(void );
extern void  Du1018dConfigurationInit(stDu1018dStatus *Status);
extern stDu1018dStatus  gDu1018dStatus ;
#endif

