#include "LocalUart.h"
#include "ymodem.h"
#include "uart.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bootloader.h"
#include "stdlib.h"
#include "my_norflash.h"
#include "ff.h"
#include "interflash.h"
#include "system_info.h"
#include "delay.h"


#define gLocalUartHandle           gstUart5.UartHandle  
#define LocalUartSendBytes         COM2_SendBytes 
#define LocalUartRecBytes(pdata,size)             do{COM2_RecBytes(pdata,size) ; while(HAL_UART_GetState((UART_HandleTypeDef*)&gLocalUartHandle)!=HAL_UART_STATE_READY ){}}while(0)
stRxDataType gstRxData;



TaskHandle_t  LocalUartTask_Handler;
#define  LOCAL_UART_TASK_PRIO   5
#define  LOCAL_UART_STK_SIZE   1024
static void LocalUartTask(void* param);
void CreateLocalUartUpdateTask(void)
{
	xTaskCreate((TaskFunction_t ) LocalUartTask,            //任务函数
			(const char*    ) "start_task",          //任务名称
			(uint16_t       )LOCAL_UART_STK_SIZE,        //任务堆栈大小
			(void*          )NULL,                  //传递给任务函数的参数
			(UBaseType_t    )LOCAL_UART_TASK_PRIO,       //任务优先级
			(TaskHandle_t*  )&LocalUartTask_Handler);   //任务句柄  				
}
void LocalUartInit(unsigned int Baud )
{ 
	//gstUart2.eRxMode=UART_RX_HEAD_TAIL;
	gLocalUartHandle.Instance= UART5;
	gLocalUartHandle.Init.BaudRate=Baud;
	gLocalUartHandle.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	gLocalUartHandle.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	gLocalUartHandle.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位 
	gLocalUartHandle.Init.Mode=UART_MODE_TX_RX;		    //
	HAL_UART_Init( (UART_HandleTypeDef*)&gLocalUartHandle); 
	COM5_RecBytes((uint8_t *)&(gstUart5.ucRecByte),1); 
}
/*
{“Version”:[
{“ModuleName”:”GNSSModule_NT_ZTE3630_EB”,
“HardVersion”:”1.0”,
“FireVersion”:”1.0.0”,
“DateTime”:”98898”}
]} 
*/
#define MODULE_NAME_POS  27
#define HARD_VERSION_POS  17
#define FIRE_VERSION_POS  17
#define DATA_TIME_POS     14
#define END_BYTE_1       ','
#define END_BYTE_2       '"'
#define END_BYTE_3       '}'
#define END_BYTE_4       ','
#define END_BYTE_5       ','

void AnalysisFireInfo(unsigned char *pData)
{
	unsigned char cnt=0;
	memset((void*)&gstUpdate.stFireInfo,0,sizeof gstUpdate.stFireInfo);
	pData+=MODULE_NAME_POS;
	for(cnt=0;pData[0]!=END_BYTE_2;cnt++)
	{
		gstUpdate.stFireInfo.ModuleName[cnt]=*pData++;
	}
	pData+=HARD_VERSION_POS;
	for(cnt=0;pData[0]!=END_BYTE_2;cnt++)
	{
		gstUpdate.stFireInfo.HardVersion[cnt]=*pData++;
	}
	pData+=FIRE_VERSION_POS;
	for(cnt=0;pData[0]!=END_BYTE_2;cnt++)
	{
		gstUpdate.stFireInfo.FireVersion[cnt]=*pData++;
	}
	pData+=DATA_TIME_POS;
	for(cnt=0;pData[0]!=END_BYTE_2;cnt++)
	{
		gstUpdate.stFireInfo.Time[cnt]=*pData++;
	}
}
void GetFireInfo(void)
{
	unsigned short len =0;
	char * pdata=malloc(1024),*pSendStr=malloc(1024);
	if(pdata==NULL)  while(1);
	if(pSendStr==NULL)  while(1);
	memset(pdata,0,1024);
	sprintf(pdata,"{\"Version\":[{\"ModuleName\":\"%s\",\"HardVersion\":\"%s\",\"FireVersion\":\"%s\",\"DataTime\":\"%s\"}]}"\
	,gstUpdate.stFireInfo.ModuleName\
	,gstUpdate.stFireInfo.HardVersion\
	,gstUpdate.stFireInfo.FireVersion\
	,gstUpdate.stFireInfo.Time); 
	len=strlen(pdata);
	//sprintf(pSendStr,"%c%d%s%c",FRAME_HEAD_CHAR,len,pdata,FRAME_TAIL_CHAR);
	LocalUartSendBytes((uint8_t*)pSendStr,strlen(pSendStr));
	free(pSendStr);
	free(pdata);
	pSendStr=NULL;
	pdata=NULL;
}
void UpdateApp(void)
{
	char ucADDR[10]={0};
	AnalysisFireInfo(gstRxData.ucData);
	//gstUart2.eRxMode=UART_RX_NONE_IT;
 //	UART2_RX_BYTE_FLAG=RESET;
	if(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)
	{
		sprintf(ucADDR,"\r\nUpdate User2APP\r\n\r\n");
		gstUpdate.eAppUpdateStatus=USR1_START_UPDATE;
	}
	else if(gstUpdate.unCurrentAppADDR==USER2_APP_ADDR)
	{
		sprintf(ucADDR,"\r\nUpdate User1APP\r\n\r\n");
		gstUpdate.eAppUpdateStatus=USR2_START_UPDATE;
	}
	else 
	{
		gstUpdate.unCurrentAppADDR=USER2_APP_ADDR;
		sprintf(ucADDR,"\r\nUpdate User1APP\r\n\r\n");
		gstUpdate.eAppUpdateStatus=USR2_START_UPDATE;
	}
	LocalUartSendBytes((uint8_t*)ucADDR,strlen(ucADDR));
	delay_ms(1000);
	gstUpdate.unCurrentAppSize= Ymodem_Receive(gucAppBuf);
	//gstUart2.eRxMode=UART_RX_HEAD_TAIL;
//	SaveAppBackup();
	//if(gstUpdate.eAppUpdateStatus==NO_UPDATE)
	{ 
		gstUpdate.unCurrentAppADDR=(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)?USER2_APP_ADDR:USER1_APP_ADDR;
 		SaveSysInfo();
		RefreshSysInfo();
		InterFlash_Program(gstUpdate.unCurrentAppADDR,gucAppBuf,gstUpdate.unCurrentAppSize, FLASH_TYPEPROGRAM_BYTE);
		RunAPP(gstUpdate.unCurrentAppADDR);
	}
//	else if(gstUpdate.eAppUpdateStatus==START_UPDATE)
//	{
//		InterFlash_Program(gstUpdate.unCurrentAppADDR,gucAppBuf,gstUpdate.unCurrentAppSize, FLASH_TYPEPROGRAM_BYTE);
//	  RunAPP(gstUpdate.unCurrentAppADDR);
//	}
}
/*
char :headpos 0 
char :cmd     1
unsigned short : len
char *:data
char :tailpos
 //  datalen : len-cmdlen
*/
void AnalysisUartData(char *pData,unsigned int Len)
{
	gstRxData.ucCmd=pData[3];
	gstRxData.usDataLen=(pData[1]<<8)+pData[2]-1;
	if(gstRxData.usDataLen<RX_DATA_LEN_MAX)
	memcpy(gstRxData.ucData,(void*)&pData[4],gstRxData.usDataLen);
} 
void LocalUartTask(void* param)
{ 
	unsigned int TimeOut=0xe000000; 
	LocalUartInit(115200);  
	while( TimeOut--)
	{
//		if(UART2_RX_FINISH_FLAG==SET)
//		{ 
//			AnalysisUartData((char*)gUSART2_Buffer,gstUart2.unUART_Buffer_Len);
//			UART2_RX_FINISH_FLAG=RESET;
//			switch(gstRxData.ucCmd)
//			{
//				case LOCAL_UART_UPDATE:
//					UpdateApp();
//					break;
//				case LOCAL_UART_GET_INFO:
//					GetFireInfo();
//					break;
//			}
//		}
	}
	RunAPP(gstUpdate.unCurrentAppADDR);	
}






