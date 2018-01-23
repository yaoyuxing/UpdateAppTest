#include "sys.h"
#include "uart6.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
/*
bound ������
*/

UART_HandleTypeDef gUART6_Handler; //UART4���
u8 gUSART6_HalRxBuffer[USART6_RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
u8 gUSART6_RX[USART6_REC_LEN];
u16 gUART6_NUM;
u8 gUSART6_RX_STATUS;

/*
����4��ʼ��
*/
void uart6_init(u32 bound)
{
	gUART6_Handler.Instance=USART6;					    //USART6
	gUART6_Handler.Init.BaudRate=bound;				    //������
	gUART6_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	gUART6_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	gUART6_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	gUART6_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	gUART6_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&gUART6_Handler);					    //HAL_UART_Init()��ʹ��UART4

	HAL_UART_Receive_IT(&gUART6_Handler, (u8 *)gUSART6_HalRxBuffer, USART6_RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������

}


//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��
#define  UART6_PIN_TX	GPIO_PIN_6 //C6
#define  UART6_PIN_RX	GPIO_PIN_7//C7

void UART6_HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART6)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART6_CLK_ENABLE();			//ʹ��USART1ʱ��
	
		GPIO_Initure.Pin=UART6_PIN_TX;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF8_USART6;	//����ΪUART4
		
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=UART6_PIN_RX;			//PA10
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//��ʼ��PA10
		
#if EN_UART6_RX
		HAL_NVIC_EnableIRQ(USART6_IRQn);		//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART6_IRQn,2,1);	//��ռ���ȼ�3�������ȼ�3
#endif	
	}

}


//����6�жϷ������
void USART6_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&gUART6_Handler);	//����HAL���жϴ����ú���
	
	timeout=0;
    while (HAL_UART_GetState(&gUART6_Handler) != HAL_UART_STATE_READY)//�ȴ�����
	{
	 timeout++;////��ʱ����
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&gUART6_Handler, (u8 *)gUSART6_HalRxBuffer, USART6_RXBUFFERSIZE) != HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
	 timeout++; //��ʱ����
	 if(timeout>maxDelay) break;	
	}
} 
/*
����4����һ��byte����
*/
u8 UART6_SendByte(u8 ch)
{ 	
	while((USART6->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART6->TDR = ch;      
	return ch;
}
/*
����4����һ���ַ���
*/
 void UART6_SendBytes(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=strlen((const char*)p_comm);
	for(num=0;num<len;num++)
	{
		UART6_SendByte(*(p_comm+num));
		delay_xms(1);
	}
}
/*
����ָ�����ȵ�����
*/
 void UART6_SendNumBytes(char *data,unsigned int len)
{
	char *p_comm=data;
	uint16_t num=0;
	for(num=0;num<len;num++)
	{
		UART6_SendByte(*(p_comm+num));
	}
}
/*
���ڽ������ݴ���ص�����
*/

void UART6_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART6)//����Ǵ���4
	{
		gUSART6_RX[gUART6_NUM++]=gUSART6_HalRxBuffer[0];

		if(gUART6_NUM>=USART6_REC_LEN)
		{
			gUSART6_RX_STATUS|=0x1;
			gUART6_NUM=0;
		}
	
	}
}




