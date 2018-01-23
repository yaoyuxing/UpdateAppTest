#ifndef __MY_TASK_H_
#define __MY_TASK_H_
#include "FreeRTOS.h"
#include "task.h"
#include "rtk.h"
/*
ȫ�ֱ�������
*/
extern stRtkStatus		gRtkStatus;
/*
��������
*/
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
extern TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);
/*
RTK����
*/
//�������ȼ�
#define START_RTK_PRIO		2
//�����ջ��С	
#define START_RTK_SIZE 		2048 
//������
extern TaskHandle_t RtkTask_Handler;
//������
extern void rtk_task(void *pvParameters);
/*
4G����
*/
//�������ȼ�
#define START_ME3630_PRIO		3
//�����ջ��С	
#define START_ME3630_SIZE 		128 
//������
extern TaskHandle_t Me3630Task_Handler;
//������
extern void me3630_task(void *pvParameters);
/*
��������
*/
//�������ȼ�
#define START_BL_PRIO		4
//�����ջ��С	
#define START_BL_SIZE 		2048 
//������
extern TaskHandle_t BLTask_Handler;
//������
extern void ble_task(void *pvParameters);

/*
OLED������ص�����
*/
//�������ȼ�
#define START_OLED_PRIO		2
//�����ջ��С	
#define START_OLED_SIZE 		1024 
//������
extern TaskHandle_t OLEDTask_Handler;
//������
extern void oled_task(void *pvParameters);

/*
RADIO������ص�����
*/
//�������ȼ�
#define START_RADIO_PRIO		5
//�����ջ��С	
#define START_RADIO_SIZE 		1024 
//������
extern TaskHandle_t RADIOTask_Handler;
//������
extern void radio_task(void *pvParameters);
#endif
