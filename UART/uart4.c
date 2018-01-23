#include "sys.h"
#include "uart4.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
#define  UART_X  UART4
/*
bound ������
*/

UART_HandleTypeDef gUART4_Handler; //UART4���
u8 gUSART4_HalRxBuffer[USART4_RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
u8 gUSART4_RX[USART4_REC_LEN];
u16 gUART4_NUM;
u8 gUSART4_RX_STATUS;

/*
����4��ʼ��
*/
void uart4_init(u32 bound)
{
	gUART4_Handler.Instance=UART_X;					    //USART2
	gUART4_Handler.Init.BaudRate=bound;				    //������
	gUART4_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	gUART4_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	gUART4_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	gUART4_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	gUART4_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&gUART4_Handler);					    //HAL_UART_Init()��ʹ��UART4

	HAL_UART_Receive_IT(&gUART4_Handler, (u8 *)gUSART4_HalRxBuffer, USART4_RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������

}


//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��
#define  UART4_PIN_TX	GPIO_PIN_11
#define  UART4_PIN_RX	GPIO_PIN_12

void UART4_HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==UART_X)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_UART4_CLK_ENABLE();			//ʹ��USART1ʱ��
	
		GPIO_Initure.Pin=UART4_PIN_TX;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF8_UART4;	//����ΪUART4
		
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=UART4_PIN_RX;			//PA10
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//��ʼ��PA10
		
#if EN_UART4_RX
		HAL_NVIC_EnableIRQ(UART4_IRQn);		//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(UART4_IRQn,2,1);	//��ռ���ȼ�3�������ȼ�3
#endif	
	}

}


//����4�жϷ������
void UART4_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&gUART4_Handler);	//����HAL���жϴ����ú���
	
	timeout=0;
    while (HAL_UART_GetState(&gUART4_Handler) != HAL_UART_STATE_READY)//�ȴ�����
	{
	 timeout++;////��ʱ����
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&gUART4_Handler, (u8 *)gUSART4_HalRxBuffer, USART4_RXBUFFERSIZE) != HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
	 timeout++; //��ʱ����
	 if(timeout>maxDelay) break;	
	}
} 
/*
����4����һ��byte����
*/
u8 UART4_SendByte(u8 ch)
{ 	
	while((UART_X->ISR&0X40)==0);//ѭ������,ֱ���������   
	UART_X->TDR = ch;      
	return ch;
}
/*
����4����ָ�����ȵ�����
*/
void UART4_SendNumBytes(char *data,unsigned int len)
{
	char *p_comm=data;
	uint16_t num=0;
	for(num=0;num<len;num++)
	{
		UART4_SendByte(*(p_comm+num));
	}
}
/*
����4����һ���ַ���
*/
 void UART4_SendBytes(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=strlen((const char*)p_comm);
	for(num=0;num<len;num++)
	{
		UART4_SendByte(*(p_comm+num));
		delay_xms(1);
	}
}
/*
���ڽ������ݴ���ص�����
*/

void UART4_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==UART_X)//����Ǵ���4
	{
		gUSART4_RX[gUART4_NUM++]=gUSART4_HalRxBuffer[0];

		if(gUART4_NUM>=USART4_REC_LEN)
		{
			gUSART4_RX_STATUS|=0x1;
			gUART4_NUM=0;
		}
	
	}
}




