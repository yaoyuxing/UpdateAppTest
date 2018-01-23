/*
��̨�����շ�����
*/
#include "sys.h"
#include "uart7.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
#include "debug.h"
/*
bound ������
*/

UART_HandleTypeDef gUART7_Handler; //UART7���
u8 	gUSART7_HalRxBuffer[USART7_RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
u8 	gUSART7_RX[USART7_REC_LEN];
u16 gUART7_NUM;
u8 	gUSART7_RX_STATUS;

/*
����7��ʼ��
*/
void UART7_init(u32 bound)
{
	gUART7_Handler.Instance=UART7;					    //USART5
	gUART7_Handler.Init.BaudRate=bound;				    //������
	gUART7_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	gUART7_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	gUART7_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	gUART7_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	gUART7_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&gUART7_Handler);					    //HAL_UART_Init()��ʹ��UART5

	HAL_UART_Receive_IT(&gUART7_Handler, (u8 *)gUSART7_HalRxBuffer, USART7_RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������

}
/*
�޸Ĳ����ʺ���
*/
void UART7_Bound(u32 bound)
{
	gUART7_Handler.Init.BaudRate=bound;				    //������
	HAL_UART_Init(&gUART7_Handler);					    //HAL_UART_Init()��ʹ��UART5
}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��
#define  UART7_PIN_TX	GPIO_PIN_7
#define  UART7_PIN_RX	GPIO_PIN_6
#define  UART7_GPIO_TX	GPIOF
#define  UART7_GPIO_RX	GPIOF

void UART7_HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==UART7)//����Ǵ���7�����д���7 MSP��ʼ��
	{
		__HAL_RCC_GPIOF_CLK_ENABLE();			//ʹ��GPIOFʱ��
		__HAL_RCC_UART7_CLK_ENABLE();			//ʹ��USART7ʱ��
	
		GPIO_Initure.Pin=UART7_PIN_TX;			
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF8_UART7;	//����ΪUART7
		
		HAL_GPIO_Init(UART7_GPIO_TX,&GPIO_Initure);	   	

		GPIO_Initure.Pin=UART7_PIN_RX;			
		HAL_GPIO_Init(UART7_GPIO_RX,&GPIO_Initure);	   	
		
#if EN_UART7_RX
		HAL_NVIC_EnableIRQ(UART7_IRQn);		//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(UART7_IRQn,2,1);	//��ռ���ȼ�3�������ȼ�3
#endif	
	}

}


//����7�жϷ������
void UART7_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&gUART7_Handler);	//����HAL���жϴ����ú���
	
	timeout=0;
    while (HAL_UART_GetState(&gUART7_Handler) != HAL_UART_STATE_READY)//�ȴ�����
	{
	 timeout++;////��ʱ����
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&gUART7_Handler, (u8 *)gUSART7_HalRxBuffer, USART7_RXBUFFERSIZE) != HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
	 timeout++; //��ʱ����
	 if(timeout>maxDelay) break;	
	}
} 
/*
����7����һ��byte����
*/
u8 UART7_SendByte(u8 ch)
{ 	
	while((UART5->ISR&0X40)==0);//ѭ������,ֱ���������   
	UART7->TDR = ch;      
	return ch;
}
/*
����7����һ���ַ���
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
����7����һ���ַ���
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
����7����һ���ڴ������
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
���ڽ������ݴ���ص�����
*/

void UART7_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==UART7)//����Ǵ���7
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


