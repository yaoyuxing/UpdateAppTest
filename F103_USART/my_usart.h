

#ifndef  MY_USART_H
#define  MY_USART_H



//����1 ѡ��MCU 
//#define MY_STM32F103      //ѡ��MCU
// #define MY_STM32F769 
//#define MY_STM32FXXX
//#define MY_STM32FXXX 
//����2  ѡ���õ��Ĵ���
#define  USE_USART1
//#define  USE_USART2
//#define  USE_USART3
//#define  USE_USART4
//#define  USE_USART5
//#define  USE_USART6
#define  USE_FREE_RTOS



//ö������
typedef enum USART_BAUD
{
	_4800 = 4800,
	_9600 = 9600,
	_115200 =115200,
	_921600 = 921600,
}eUSART_BaudType;
typedef enum USART_PORT
{
	MY_USART1=1,
	MY_USART2,
	MY_USART3,
	MY_USART4,
	MY_USART5,
	MY_USART6,
	MY_USART7,
}eUSART_PORTType;
typedef enum State
{
	USART_CLOSE =0,
	USART_ENABLE =!0,
}eUSART_StatType;
typedef enum usart_rec_int
{
	REC_INT_ENABLE =0x10,
	REC_INT_DISABLE =0x00,
	USART_PRIO_0=0x00,
	USART_PRIO_1=0x01,
	USART_PRIO_2,
	USART_PRIO_3,
	USART_PRIO_4,
	USART_PRIO_5,
	USART_PRIO_6,
	USART_PRIO_7,
	USART_PRIO_8,
	USART_PRIO_9,
	USART_PRIO_10,
	USART_PRIO_11,
	USART_PRIO_12,
	USART_PRIO_13,
	USART_PRIO_14,
	USART_PRIO_15, 
}eUSART_RecIntType;

#define  USART_REC_BUF_MAX      1024*1

typedef struct usart_struct
{
	unsigned char Flags[8];
	unsigned int  RecFrameLen;
	char          RecBuf[USART_REC_BUF_MAX];
}sUSART_Type;
extern   volatile sUSART_Type  gsUSART;          //���ڽṹ

#define  USART1_ISR_REC_TIMEOUT_FLAG    gsUSART.Flags[0]   //����һ���ճ�ʱ��־
#define  USART1_ISR_REC_START_FLAG      gsUSART.Flags[1]   //����һ���տ�ʼ��־
#define  USART1_ISR_REC_END_FLAG        gsUSART.Flags[2]   //����һ���ս�����־



#ifdef   USE_FREE_RTOS

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

extern void usart_rec_timer_callback(TimerHandle_t xtimer);
extern  TimerHandle_t                usart1_rec_timer;   
#endif 
 
#ifdef   MY_STM32F103
#include  "stm32f10x.h"   //ͷ�ļ�ѡ��
/***********��������*************/ 
//stm32f103
/*******************
103���ڳ�ʼ������
����һ: eUsart���ڱ��
��������eRecIntSta�����ж�����/�ж����ȼ�
��������eBaud������  
�����ģ�eStatʹ�ܱ�־
********************/
void MY_F103_USART_Init(eUSART_PORTType eUsart ,eUSART_RecIntType eRecIntSta,eUSART_BaudType eBaud,eUSART_StatType eStat);

/*******************
103���ڷ��ͺ���
����һ�����ڱ��
������: ����bufָ��
���������������ݳ��� 
********************/
void MY_F103_USART_SendBytes(USART_TypeDef    *USARTx,const char * pSendBuf,unsigned int cnt);

/*******************
103���ڷ��ͺ���
����һ�����ڱ��
������: ����bufָ��
���������������ݳ��� 
********************/
void MY_F103_USART_RecBytes(USART_TypeDef    *USARTx,char *pRecBuf,unsigned int cnt);

/*******************
103�жϽ���������غ���
����һ����ͷһ���ֽ�
��������֡βһ���ֽ� 
����ֵ��1�ɹ� 0 ���ڽ���
********************/
int  USART1_Interrupt_SoftFlowCtrl(const char FrameHead,const char FrameTail);
/**********************************
103�ж�ʱ�����֡���պ���
���ճ�ʱ�������ⲿ��ʱ������
USART1_ISR_REC_TIMEOUT_FLAG��ʱ��־
���֡����
*********************************/
void  USART1_Interrupt_TimerFrameCtrl(void); 


#define  MY_USART_Init        MY_F103_USART_Init
#define  MY_USART_SendBytes   MY_F103_USART_SendBytes
#define  MY_USART_RecBytes    MY_F103_USART_RecBytes
#ifdef   USE_USART1

#define    Debug_SendBytes(ucData)   do{USART_SendData(USART1,ucData);while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);}while(0)
//#define    Debug_SendBytes(ucData)   ( void *)0
#endif
 
#endif

 

#ifdef   MY_STM32F769
#include  "stm32f7xx.h"   //ͷ�ļ�ѡ��
/***********��������*************/ 
//stm32f103
/*******************
103���ڳ�ʼ������
����һ: eUsart���ڱ��
��������eRecIntSta�����ж�����/�ж����ȼ�
��������eBaud������  
�����ģ�eStatʹ�ܱ�־
********************/
void MY_F769_USART_Init(eUSART_PORTType eUsart ,eUSART_RecIntType eRecIntSta,eUSART_BaudType eBaud,eUSART_StatType eStat);

/*******************
103���ڷ��ͺ���
����һ�����ڱ��
������: ����bufָ��
���������������ݳ��� 
********************/
void MY_F769_USART_SendBytes(USART_TypeDef    *USARTx,const char * pSendBuf,unsigned int cnt);

/*******************
103���ڷ��ͺ���
����һ�����ڱ��
������: ����bufָ��
���������������ݳ��� 
********************/
void MY_F769_USART_RecBytes(USART_TypeDef    *USARTx,char *pRecBuf,unsigned int cnt);

/*******************
103�жϽ���������غ���
����һ����ͷһ���ֽ�
��������֡βһ���ֽ� 
����ֵ��1�ɹ� 0 ���ڽ���
********************/
int  USART1_Interrupt_SoftFlowCtrl(const char FrameHead,const char FrameTail);
/**********************************
103�ж�ʱ�����֡���պ���
���ճ�ʱ�������ⲿ��ʱ������
USART1_ISR_REC_TIMEOUT_FLAG��ʱ��־
���֡����
*********************************/
void  USART1_Interrupt_TimerFrameCtrl(void); 
#define  MY_USART_Init        MY_F769_USART_Init
#define  MY_USART_SendBytes   MY_F769_USART_SendBytes
#define  MY_USART_RecBytes    MY_F769_USART_RecBytes 
 
#ifdef   USE_USART1 
#define    Debug_SendBytes(ucData)   ( void *)0 
#else
#define    Debug_SendBytes(ucData)   ( void *)0 
#endif

#endif

//#define  DEBUG
#define  DEBUG_FORMAT_BUF     200
void DEBUG_PRINTF(const char *fmt,...);


#endif



