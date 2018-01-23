#include "sys.h"
#include "delay.h"  
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "LocalUart.h"
#include "ff.h"
#include "my_norflash.h"
#include "interflash.h" 
#include "uart.h"
#include "LocalUart.h"
#include "bootloader.h"
#include "string.h"
#include "system_info.h"

unsigned char gucAppBuf[APP_BUF_MAX];
TaskHandle_t  StartTask_Handler;
#define  START_TASK_PRIO   5
#define  START_STK_SIZE   1024
void start_task(void *param)
{
	 taskENTER_CRITICAL();           //�����ٽ���
	#ifdef INTER_FLASH_TEST
	ceate_interflash_test();
	#endif
	#ifdef NOR_FLASH_FS_TEST
	ceate_fs_test();
	#endif
	#ifdef USART_TEST
	usart_test_fun();
	#endif
	
	 CreateLocalUartUpdateTask();
	 taskEXIT_CRITICAL();           //�����ٽ���
	 vTaskDelete(StartTask_Handler); //ɾ����ʼ����
}
uint8_t buf[1024]="abcdef1234";
int main(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);                 //�жϷ���4 0~15��ռ��ʽ
	MPU_ConfigNOR();
	Cache_Enable();                 //��L1-Cache
	HAL_Init();                     //��ʼ��HAL��   
	Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 

	delay_init(216);                //��ʱ��ʼ��
	BSP_NOR_Init();   
		GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;		//�����������
	GPIO_InitStruct.Pull=GPIO_PULLUP;			//����
	GPIO_InitStruct.Speed=GPIO_SPEED_FAST;		//����
	__HAL_RCC_GPIOG_CLK_ENABLE();
		GPIO_InitStruct.Pin=GPIO_PIN_12|GPIO_PIN_11; 
	HAL_GPIO_Init(GPIOG,&GPIO_InitStruct);	 
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12|GPIO_PIN_11,1); 
	delay_ms(1000);
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12|GPIO_PIN_11,0); 
	
	
	LocalUartInit(115200);
    SystemInfoInit_App(buf,4);
	//AppCallFunction();
	UpdateInit_App();
	
	
	while(1)
	{ 
		//COM5_RecBytes(buf,1); 
	//	while(HAL_UART_GetState(&gstUart5.UartHandle)!=HAL_UART_STATE_READY);
  	//if(memcmp(buf,"start",5)==0)
		if(UART5_RX_GOT_BYTE_FLAG)
		{
			UART5_RX_GOT_BYTE_FLAG=0;
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12|GPIO_PIN_11,1);
			delay_ms(2000);
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12|GPIO_PIN_11,0); 
		}
	}
    //������ʼ����
    xTaskCreate((TaskFunction_t ) start_task,            //������
                (const char*    ) "start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������                
    vTaskStartScheduler();          //�����������
}







