#include "my_task.h"
#include "rtk.h"
#include "me3630.h"
#include "json_test.h"
#include "MySys.h"
#include "string.h"
#include "debug.h"

#include "led.h"
#include "oled.h"
#include "ChineseCharacter.h"
#include "MyGui.h"
#include "delay.h"

#include "bluetooth.h"
#include <stdlib.h>

#include "radio.h"
#include "HX_DU1018D.h" 

#include "my_norflash.h"





extern void LED(u8 n);
//任务句柄
TaskHandle_t StartTask_Handler;
TaskHandle_t RtkTask_Handler;
TaskHandle_t Me3630Task_Handler;
TaskHandle_t BLTask_Handler;
TaskHandle_t OLEDTask_Handler;
TaskHandle_t RADIOTask_Handler;
 
//开始任务任务函数
char  fsdata[512]={  0
};
 
  

void start_task(void *pvParameters)
{
	  
    taskENTER_CRITICAL();           //进入临界区
    //创建RTK任务
    xTaskCreate((TaskFunction_t )rtk_task,             
                (const char*    )"RTK",           
                (uint16_t       )START_RTK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )START_RTK_PRIO,        
                (TaskHandle_t*  )&RtkTask_Handler);   
//	//创建me3630（4G数据任务）
//	xTaskCreate((TaskFunction_t )me3630_task,             
//                (const char*    )"ME3630",           
//                (uint16_t       )START_ME3630_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )START_ME3630_PRIO,        
//                (TaskHandle_t*  )&Me3630Task_Handler); 
	//创建蓝牙任务
	xTaskCreate(( TaskFunction_t )ble_task,             
                (const char*    )"BLUETOOTH",           
                (uint16_t       )START_BL_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )START_BL_PRIO,        
                (TaskHandle_t*  )&BLTask_Handler); 
//	
	//创建OLED任务
	xTaskCreate((TaskFunction_t )oled_task,             
                (const char*    )"OLED",           
                (uint16_t       )START_OLED_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )START_OLED_PRIO,        
                (TaskHandle_t*  )&OLEDTask_Handler); 
	//创建RADIO任务
	xTaskCreate((TaskFunction_t )radio_task,             
                (const char*    )"RADIO",           
                (uint16_t       )START_RADIO_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )START_RADIO_PRIO,        
                (TaskHandle_t*  )&RADIOTask_Handler); 
 
#ifdef NOR_FLASH_FS_TEST
		ceate_fs_test();			
#endif								
								
	   taskEXIT_CRITICAL();            //退出临界区 
 			 	
    vTaskDelete(StartTask_Handler); //删除开始任务
   
}
/*

*/
#include "uart6.h"
void rtk_task(void *pvParameters)
{
	RtkStatusInit(&gRtkStatus);
	RtkInit(&gRtkStatus);
	
	while(1)
	{
		#if GGA_TASK_TEST
			printf("task ok \r\n");
		#else
			RtkGetGga(&gRtkStatus,&Me3630Status);
		#endif
			vTaskDelay(1000);      
	}

}
/*
4g模块任务
*/


void me3630_task(void *pvParameters)
{
	int time_num=0;
	Me3630StatusInit(&Me3630Status);
	openTCP(&Me3630Status);
	while(1)
	{
		if(time_num==50)
		{
			#if GGA_TASK_TEST
				cJSON_GgaToJson("$GPGGA,000001,3112.518576,N,12127.901251,E,1,8,1,0,M,-32,M,3,0*4B#1");
			#else
			if(gRtkStatus.bit.get_gga==eGetGgaY)
			{
				#if 1
					#define Hardware_ID  "#1"
				#endif				
				strncat(gSysGgaDataBuffer,Hardware_ID,strlen(Hardware_ID));
				#if	DEBUG_TASK
					printf("cJSON_GgaToJson=%s",gSysGgaDataBuffer);
				#endif

				cJSON_GgaToJson(gSysGgaDataBuffer);

				gRtkStatus.bit.get_gga=eGetGgaN;
			}
			#endif
			time_num=0;
		}
		
		me3630RxUnpacking(&Me3630Status);
		time_num++;
		vTaskDelay(1); 
	}
}
/*
OLED显示的任务
*/
void oled_task(void *pvParameters)
{

	u8 num=0;
	u8 duf[512];
    u8 t=0; 
    LED_Init();                     //初始化LED 
    OLED_Init();	                //初始化OLED
	OLED_Refresh_Gram();//更新显示到OLED	 
    while(1)
    {
		OLED_Clear()  ;
#if 0
		GUI_4GSignalStrengthDynamic(num%5,0,2);
		GUI_BatteryPower(num++);
		GUI_Wireframe();
		GUI_BleWorkStatus(num%2);
		GUI_WorkMode(num%3);
		GUI_EquipmentWorkStatus(num%2);
		if(num==10)
			num=0;
#else
		memset(duf,0,512);
		GUI_Wireframe();//线宽
		GUI_BleWorkStatus((eGUI_BleStatus)BL_CONNECTION_STATUS());//蓝牙
		GUI_4GSignalStrengthDynamic(num%5,0,2);//4G信号强度
		GUI_WorkMode(eGUI_BaseStationMode);//模式切换
		GUI_EquipmentWorkStatus(eEquipmentWorkStatussY);//工作状态
//		
		GUI_BatteryPower(num++);
		CUI_RTKStatus(CUI_RTKStatus_X_START,CUI_RTKStatus_Y_START,gRtkStatus);
		CUI_RadioChannel(CUI_RadioChannel_X,CUI_RadioChannel_Y,gDu1018dStatus);
		if(num==10)
			num=0;
#endif		
	
		
		OLED_Refresh_Gram();        //更新显示到OLED
		
		vTaskDelay(100); 
//		delay_ms(1000);
//		LED0=!LED0;
	} 
}

/*
蓝牙的任务
*/
void ble_task(void *pvParameters)
{
 	char **data_return=0;
	*data_return=0;

	stBlDataHead 	BlDataHead;
	BlInit(&gBlStatus);//初始化蓝牙模块，
    while(1)
    {
		//建立状态监控：如果断开连接记录断连并重新连接
		if(BL_CONNECTION_STATUS())//在运行过程中断连了
		{
			gBlRtcmOpenFlag=0;
			BlReset();
			BlInit(&gBlStatus);
		} 
		BluetoothAnalysis(data_return,&gBlStatus);
		if(gBlStatus.bit.rx_buff_analysis==eBlRxBuffAnalysisY)
		{
			BlReceiveDataUnpacking(data_return,&gBlStatus,&gRtkStatus);
			gBlStatus.bit.rx_buff_analysis=eBlRxBuffAnalysisN;
		}
		free(*data_return);
		delay_ms(10);
	} 
}

/*
电台任务
*/

#define	 HX_DU1018D 1
void radio_task(void *pvParameters)
{
#if HX_DU1018D
	u8 channel='6';
	u8	re_channel=0;
	UART7_init(9600);
	Du1018dConfigurationInit(&gDu1018dStatus);

	Du1018dSetChannel(&channel,1,&gDu1018dStatus);
	Du1018dReadChannel(&re_channel,&gDu1018dStatus);
	while(1)
	{
		if(gRtkStatus.bit.gpgga_status!=eGpggaStatus7)
		{
			RadioDataSend(&gRadioCommandParameter,gSysGgaDataBuffer);
		}
		delay_ms(500);
	}
	
#else
	UART7_init(9600);
	RadioInit(&gRadioStatus,&gRadioCommandParameter);	
	while(1)
	{
		if(gRtkStatus.bit.gpgga_status!=eGpggaStatus7)
		{
			RadioDataSend(&gRadioCommandParameter,gSysGgaDataBuffer);
		}
		delay_ms(500);
	}
#endif
}


