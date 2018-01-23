
#include "my_usart.h"
#include "stdarg.h" 
#include "stdio.h"
#include "string.h"
#include "task.h"
/**********************************
���ô��ڲ��裺
1��"usart_config.h"���ݺ�ѡ��MCU 
2��"usart_config.h"ѡ����Ҫ�õ���USART
3�����ĺ�ѡ����GPIO�ܽ�
4��MY_USART_Init ��ʼ������
5�����д��ڽ����жϿ�ѡ�����ַ�ʽ 
   a��USART_Interrupt_SoftFlowCtrl ������ط�ʽ����֡����ͷ��β��һ���ֽڣ����ⲿ������� USART1_ISR_REC_TIMEOUT_FLAG �����ý��ճ�ʱ
	 b��USART_Interrupt_TimerFrameCtrl ʱ����ƽ���֡(�ⲿ��ʱ������extern����USART1_ISR_REC_TIMEOUT_FLAGRESET���ս���)
6��ѡ���Ƿ���ϵͳ 
"usart_config.h"ѡ����Լ����ڴ�ӡ�˿�
MY_USART_SendBytes ���͹̶���������
MY_USART_RecBytes  ���չ̶����Ⱥ���
DEBUG_PRINTF       ���Ը�ʽ����ӡ
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
	 DEBUG_PRINTF("�������\r\n" ); 
   //MY_USART_SendBytes(USART1,(const char *)gsUSART.RecBuf,gsUSART.RecFrameLen); 
}  
#endif


#ifdef MY_STM32F103  

//����3 ���ĺ�ѡ����GPIO�ܽ�

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
	USART_InitTypeDef USART_InitStructure;  //���ڳ�ʼ���ṹ
	GPIO_InitTypeDef GPIO_InitStructure;    //GPIO��ʼ���ṹ
  NVIC_InitTypeDef NVIC_InitStructure;    //�ж����ýṹ
  USART_TypeDef    *USARTx;
	
  USART_InitStructure.USART_BaudRate =eBaud;            //��Ʋ�����
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;         //Ĭ��8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;             //Ĭ��ֹͣλ1
  USART_InitStructure.USART_Parity = USART_Parity_No;               //Ĭ�ϲ�У��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;      //Ĭ����Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;               //Ĭ�Ͽ�TX RX 

	switch(eUsart)                   //����TX��RX GPIO
	{
		case MY_USART1:
			        USARTx=USART1;                                                      //���ڼĴ���ָ��
		          NVIC_InitStructure.NVIC_IRQChannel =USART1_IRQn;                    //�����ж�ͨ��    USART1_IRQn
							RCC_APB2PeriphClockCmd(USART1_GPIO_RCC|RCC_APB2Periph_USART1, ENABLE);  //GPIOʱ��ʹ�� ����1ʱ��ʹ��   
							GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		          GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
							GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
							GPIO_Init(USART1_PORT, &GPIO_InitStructure);                        //��ʼ�� IO
		          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		          GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
							GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
							GPIO_Init(USART1_PORT, &GPIO_InitStructure);                        //��ʼ�� IO
		          USART_Init(USARTx, &USART_InitStructure);                           //��ʼ�� USART 
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
	USART_Cmd(USARTx, ENABLE);             //��������
	
	if((eRecIntSta&0xf0)==REC_INT_ENABLE)       //ȡ����λ���ý����ж�
	{   
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =eRecIntSta&0x0f;      //ȡ����λ���ȼ�     
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
		USART_SendData(USARTx,(uint16_t)(*pSendBuf));                      //����һ���ֽ�
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
#ifdef   USE_USART1               //���ʹ��USART1�������
int  USART1_Interrupt_SoftFlowCtrl(const char FrameHead,const char FrameTail) //����1��������ƺ���
{
	 char byte;
	 if( USART_GetITStatus(USART1,USART_IT_RXNE)==SET)       //����1���ռĴ�����Ϊ��
	 {
		 byte=USART_ReceiveData(USART1);                       //�������ݵ�byte
		 if(USART1_ISR_REC_TIMEOUT_FLAG==SET)                 //��ʱ��ֵ���ճ��ȼ���λ���տ�ʼ�ͳ�ʱ��־
		 { 
			 USART1_ISR_REC_START_FLAG=RESET;                   
			 USART1_ISR_REC_TIMEOUT_FLAG=RESET; 
			 gsUSART.RecFrameLen=0;                             //��������
		 }
		 if((byte==FrameHead)&&(USART1_ISR_REC_START_FLAG==RESET))          //�ж�֡ͷ
		 {
			 USART1_ISR_REC_START_FLAG=SET;                                   //���տ�ʼ��־��λ
			 gsUSART.RecFrameLen=0;                                           //��������
			 gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;                      //����֡ͷ
		 }	
		 else if((USART1_ISR_REC_START_FLAG==SET)&&(byte!=FrameTail)	)	    //�ж�֡β
		 {
			gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;                      //����֡
		 }			 
		 else  if((USART1_ISR_REC_START_FLAG==SET)&&(byte==FrameTail)	)	     //���ս���
		 {
			 gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;                       //����֡β
			 USART1_ISR_REC_START_FLAG=RESET;                                  //��λ֡��ʼ��־
			 USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			 USART1_ISR_REC_END_FLAG=SET;                                      //������־��λ
			 return 1;
		 }
	 } 
	 USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	 return 0;
} 
void  USART1_Interrupt_TimerFrameCtrl(void)        //���Ƴ�ʱ֡���պ���
{
	 char byte;
	 if( USART_GetITStatus(USART1,USART_IT_RXNE)==SET)            //���ռĴ�����Ϊ�ձ�־��λ
	 {
		 byte=USART_ReceiveData(USART1);                            //��������
		 if(USART1_ISR_REC_START_FLAG==RESET)
		 {
			 	 USART1_ISR_REC_START_FLAG=SET;
			   USART1_ISR_REC_TIMEOUT_FLAG=RESET;
#ifdef USE_FREE_RTOS   
  	 	 xTimerStartFromISR(usart1_rec_timer,(BaseType_t *)&usart1_rec_timer_woken);         //���տ�ʼ ���ռ�ʱ��ʼ  
			 gsUSART.RecFrameLen=0;            //������� 
#endif
		 }
		 if((USART1_ISR_REC_TIMEOUT_FLAG!=SET) &&(USART1_ISR_REC_START_FLAG==SET))                      //����ʱ��δ��ʱ
		 {
			  gsUSART.RecBuf[gsUSART.RecFrameLen++]=byte;             //��������
		 } 
	 }
	 USART_ClearITPendingBit(USART1,USART_IT_RXNE);         //�����־
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





