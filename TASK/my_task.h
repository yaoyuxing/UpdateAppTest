#ifndef __MY_TASK_H_
#define __MY_TASK_H_
#include "FreeRTOS.h"
#include "task.h"
#include "rtk.h"
/*
全局变量定义
*/
extern stRtkStatus		gRtkStatus;
/*
启动任务
*/
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
extern TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);
/*
RTK任务
*/
//任务优先级
#define START_RTK_PRIO		2
//任务堆栈大小	
#define START_RTK_SIZE 		2048 
//任务句柄
extern TaskHandle_t RtkTask_Handler;
//任务函数
extern void rtk_task(void *pvParameters);
/*
4G任务
*/
//任务优先级
#define START_ME3630_PRIO		3
//任务堆栈大小	
#define START_ME3630_SIZE 		128 
//任务句柄
extern TaskHandle_t Me3630Task_Handler;
//任务函数
extern void me3630_task(void *pvParameters);
/*
蓝牙任务
*/
//任务优先级
#define START_BL_PRIO		4
//任务堆栈大小	
#define START_BL_SIZE 		2048 
//任务句柄
extern TaskHandle_t BLTask_Handler;
//任务函数
extern void ble_task(void *pvParameters);

/*
OLED任务相关的设置
*/
//任务优先级
#define START_OLED_PRIO		2
//任务堆栈大小	
#define START_OLED_SIZE 		1024 
//任务句柄
extern TaskHandle_t OLEDTask_Handler;
//任务函数
extern void oled_task(void *pvParameters);

/*
RADIO任务相关的设置
*/
//任务优先级
#define START_RADIO_PRIO		5
//任务堆栈大小	
#define START_RADIO_SIZE 		1024 
//任务句柄
extern TaskHandle_t RADIOTask_Handler;
//任务函数
extern void radio_task(void *pvParameters);
#endif
