#ifndef __HX_DU1018D_H_
#define __HX_DU1018D_H_
#include "uart7.h"
#include "sys.h"
#include "HX_DU1018D_AT.h"
#define	Du1018d_RX_BUF_LEN		USART7_REC_LEN
#define	Du1018d_RX_BUF 			gUSART7_RX
#define	Du1018d_RX_BUF_NUM 		gUART7_NUM
#define	Du1018d_RX_STATUS		gUSART7_RX_STATUS

#define	Du1018dSendByteS(X)				UART7_SendBytes(X)	//���Ͷ���ֽڵ�����  ��'\0'����
#define	Du1018dSendSizeof(X) 			UART7_SendBytesSizeof(X)//����һ���ڴ�
#define	Du1018dSendByte(X)				UART7_SendByte(X)	//����һ���ֽ�
#define	Du1018dSendByteSLen(data ,len )	UART7_SendBytesLen(data ,len)	//����һ���ֽ�
#define	Du1018dUartBound(X)				UART7_Bound(X)		//���õ�̨��Ӧ�Ĵ��ڵĲ�����
/*

*/


/*
DU1018Dģʽ�л����Ŷ���
*/
#define	DU1018D_MODE_PIN		GPIO_PIN_3
#define	DU1018D_MODE_GPIO		GPIOC
#define DU1018D_MODE(n)  		(n?HAL_GPIO_WritePin(DU1018D_MODE_GPIO,DU1018D_MODE_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(DU1018D_MODE_GPIO,DU1018D_MODE_PIN,GPIO_PIN_RESET))
						  
typedef enum  eDU1018D_MODE
{
	eDU1018D_MODE_Configuration=0 , //����
	eDU1018D_MODE_Ordinary			, //��ͨ
	
}eDU1018D_MODE;						  


typedef  struct 
{
	uint32_t CommandSend:1;//�����Ƿ��ͳɹ� 
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
	eDU1018D_CommandSendY = 0, //�ɹ� 
	eDU1018D_CommandSendN, 
}eDU1018D_CommandSend;		
/*
�����������һ������ȴ����Է��ͳɹ�������ɹ��ط�
command:ƴ�ӵ�����
parameter������Ĳ���
parameterY_N����û�в����ı��
Du1018dStatus:��������Ƿ��ͳɹ�
*/
extern void Du1018dSendCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,stDu1018dStatus *Du1018dStatus);

/*
����ͨ����
*/
extern void Du1018dSetChannel(u8 *channel,u8 data_len,stDu1018dStatus *Du1018dStatus);
/*
��ͨ����
*/
extern void Du1018dReadChannel(u8 *channel,stDu1018dStatus *Du1018dStatus);
extern void Du1018d_Test(void );
extern void  Du1018dConfigurationInit(stDu1018dStatus *Status);
extern stDu1018dStatus  gDu1018dStatus ;
#endif

