#include "sys.h"
#include "uart5.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
#include "debug.h"
/*
bound ������
*/

UART_HandleTypeDef gUART5_Handler; //UART5���
u8 gUSART5_HalRxBuffer[USART5_RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
u8 gUSART5_RX[USART5_REC_LEN];
u16 gUART5_NUM;
u8 gUSART5_RX_STATUS;

/*
����4��ʼ��
*/
void uart5_init(u32 bound)
{
	gUART5_Handler.Instance=UART5;					    //USART5
	gUART5_Handler.Init.BaudRate=bound;				    //������
	gUART5_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	gUART5_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	gUART5_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	gUART5_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	gUART5_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&gUART5_Handler);					    //HAL_UART_Init()��ʹ��UART5

	HAL_UART_Receive_IT(&gUART5_Handler, (u8 *)gUSART5_HalRxBuffer, USART5_RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������

}
#if USART5_DEVELOPMENT_BOARD

	//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
	//�˺����ᱻHAL_UART_Init()����
	//huart:���ھ��
	#define  UART5_PIN_TX	GPIO_PIN_12
	#define  UART5_PIN_RX	GPIO_PIN_2

	void UART5_HAL_UART_MspInit(UART_HandleTypeDef *huart)
	{
		//GPIO�˿�����
		GPIO_InitTypeDef GPIO_Initure;
		
		if(huart->Instance==UART5)//����Ǵ���1�����д���1 MSP��ʼ��
		{
			__HAL_RCC_GPIOC_CLK_ENABLE();			//ʹ��GPIOAʱ��
			__HAL_RCC_GPIOD_CLK_ENABLE();			//ʹ��GPIOAʱ��
			__HAL_RCC_UART5_CLK_ENABLE();			//ʹ��USART1ʱ��
		
			GPIO_Initure.Pin=UART5_PIN_TX;			//PC12
			GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
			GPIO_Initure.Pull=GPIO_PULLUP;			//����
			GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
			GPIO_Initure.Alternate=GPIO_AF8_UART5;	//����ΪUART5
			
			HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//��ʼ��PA9

			GPIO_Initure.Pin=UART5_PIN_RX;			//PD2
			HAL_GPIO_Init(GPIOD,&GPIO_Initure);	   	//��ʼ��PA10
			
		#if EN_UART5_RX
			HAL_NVIC_EnableIRQ(UART5_IRQn);		//ʹ��USART1�ж�ͨ��
			HAL_NVIC_SetPriority(UART5_IRQn,2,1);	//��ռ���ȼ�3�������ȼ�3
		#endif	
		}

	}
#else 
	//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
	//�˺����ᱻHAL_UART_Init()����
	//huart:���ھ��
	#define  UART5_PIN_TX	GPIO_PIN_13
	#define  UART5_PIN_RX	GPIO_PIN_12

	void UART5_HAL_UART_MspInit(UART_HandleTypeDef *huart)
	{
		//GPIO�˿�����
		GPIO_InitTypeDef GPIO_Initure;
		
		if(huart->Instance==UART5)//����Ǵ���5�����д���5 MSP��ʼ��
		{
			__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOBʱ��
			__HAL_RCC_UART5_CLK_ENABLE();			//ʹ��USART1ʱ��
		
			GPIO_Initure.Pin=UART5_PIN_TX|UART5_PIN_RX;			
			GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
			GPIO_Initure.Pull=GPIO_PULLUP;			//����
			GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
			GPIO_Initure.Alternate=GPIO_AF8_UART5;	//����ΪUART5
			HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��P
			
		#if EN_UART5_RX
			HAL_NVIC_EnableIRQ(UART5_IRQn);		//ʹ��USART1�ж�ͨ��
			HAL_NVIC_SetPriority(UART5_IRQn,2,1);	//��ռ���ȼ�3�������ȼ�3
		#endif	
		}

	}
#endif

//����5�жϷ������
void UART5_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&gUART5_Handler);	//����HAL���жϴ����ú���
	
	timeout=0;
    while (HAL_UART_GetState(&gUART5_Handler) != HAL_UART_STATE_READY)//�ȴ�����
	{
	 timeout++;////��ʱ����
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&gUART5_Handler, (u8 *)gUSART5_HalRxBuffer, USART5_RXBUFFERSIZE) != HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
	 timeout++; //��ʱ����
	 if(timeout>maxDelay) break;	
	}
} 
/*
����5����һ��byte����
*/
u8 UART5_SendByte(u8 ch)
{ 	
	while((UART5->ISR&0X40)==0);//ѭ������,ֱ���������   
	UART5->TDR = ch;      
	return ch;
}
/*
����5����һ���ַ���
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
����5����һ���ڴ������
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
����ָ�����ȵ�����
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
���ڽ������ݴ���ص�����
*/
#include "bluetooth.h"
#include "rtk.h"
extern char gTestFlag;
void UART5_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static char escape_falg=0,num=0;
	
	if(huart->Instance==UART5)//����Ǵ���4
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
		if(gBlRtcmOpenFlag==1&&gUSART5_HalRxBuffer[0]!='#')//���յ���rtcm����
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




