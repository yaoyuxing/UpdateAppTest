#include "ble_update.h" 
#include "stm32f7xx.h"
#include "uart.h"
#include "system_info.h"
#include "FreeRTOS.h"
#include "task.h" 
#include "delay.h"
#include "string.h"
#include "bootloader.h"
#include "ble_tooth.h"
#include "system_info.h"
#include "interflash.h"   
#include "stdlib.h"
#include "ymodem.h" 




stBleDataType  gstBleData; //蓝牙变量
TaskHandle_t  BleTask_Handler;
#define  BLE_TASK_PRIO   5
#define  BLE_STK_SIZE   1024 


static void BleUartInit(unsigned int Baud );
static void BluetoothGPIOInit(void );
static void BleReset(void);
static void BleInit(void);
static eBleErrorType BleBaudSet(unsigned int Baud);
static void BleTask(void* param);
static void BleAnalysisData(char *pData,unsigned int Len);
static void BleUpdateApp(void);
static void BleGetFireInfo(void);
static void BleAnalysisFireInfo(unsigned char *pData);
 

void CreateBleTask(void)
{
	xTaskCreate((TaskFunction_t )BleTask,            //任务函数
				(const char*    )"ble_task",         //任务名称
				(uint16_t       )BLE_STK_SIZE,        //任务堆栈大小
				(void*          )NULL,                //传递给任务函数的参数
				(UBaseType_t    )BLE_TASK_PRIO,       //任务优先级
				(TaskHandle_t*  )&BleTask_Handler);   //任务句柄  	 	
}
static void BleTask(void* param)
{  
	unsigned int TimeOut=0xe000000;  
	#if 0
	BleInit();    //初始化蓝牙模块 
	#else
	BleUartInit(115200);                    //串口波特率设置为上次存入的波特率 
	BleSendStr("sadsad");
	gBleConnectStatus=SET;
	#endif
	while(gBleConnectStatus)       //初始化连接成功后等待手机命令
	{ 
		#if 0
		if(BL_CONNECTION_STATUS())//在运行过程中断连了
		{ 
			BleInit();         //重新连接
		} 
		TimeOut--; 
		#endif
		
		if(BLE_RX_FRAME_FLAG==SET)      //接收到蓝牙串口数据帧
		{ 
			BleAnalysisData((char*)gBleRxBuffer,gBleRxBufferLen);
			BLE_RX_FRAME_FLAG=RESET;
			switch(gstBleData.ucCmd)
			{
				case BLE_CMD_UPDATE:
					BleUpdateApp();    //升级
					break;
				case BLE_CMD_GET_INFO:
					BleGetFireInfo(); //获取信息
					break;
			}
		}
	} 
	RunAPP(gstUpdate.unCurrentAppADDR);	//没有升级直接跳转
} 
/**
  * @brief  蓝牙初始化
  */
static void BleInit(void)
{
	unsigned int TimeOut=0xe000000;  
	unsigned char ucTimes=BLE_CMD_TRY_TIMES;
	int res=0; 
	BluetoothGPIOInit();
	gBleConnectStatus=RESET;
	gBleCurrentMode=BLE_CMD_MODE_NONE;
	if((gBleBaud!=BLE_DEFAULT_BAUD)&&(gBleBaud!=BLE_PENETRATE_MODE_BAUD))//读出来的波特率不正确
	{
		gBleBaud=BLE_DEFAULT_BAUD;//设置为蓝牙默认波特率
	}	
	BleUartInit(gBleBaud);                    //串口波特率设置为上次存入的波特率 
	if(BL_CONNECTION_STATUS()!=0) //未连接
	{	
		BleReset();        //重启  
		BL_MRDY_PIN_RESET();  //切换到RX接收模式
        res=BleBaudSet(BLE_PENETRATE_MODE_BAUD);    //设置为透传的波特率
		if(res!=0) 
		{
			BlePrintf("ble baud set failed res =%d\r\n",res);
		} 
		//res=BleToothSendCmd(BL_NAME,BLE_CMD_READ,"yyx");
		if(res!=0) 
		{
			while(1);
		} 
		//res=BleToothSendCmd(BL_TXPWR,BLE_CMD_SET,"-3");    //设置发射强度
		if(res!=0)        
		{
			BlePrintf("ble txpwr failed res =%d\r\n",res);
		}
		while(TimeOut--)          //连接
		{
			if(!BL_CONNECTION_STATUS()) //有设备连接上
			{
				BleDelay_ms(200);
				if(!BL_CONNECTION_STATUS())//延时防抖判断
				{
					while(ucTimes--)
					{
						res=BleToothSendCmd(BL_MODE,BLE_CMD_SET,NULL);       //发射透传命令
						if((res==BLE_NO_ERR)&&(gBleCurrentMode==BLE_PENETRATE_TRANS_MODE))//进入透传模式
						{
							gBleConnectStatus=SET;           //设置为透传
							BlePrintf("ble connected\r\n");
							break;
						}
						else
						{
							BlePrintf("ble connected but set mode err=%d\r\n",res);
						}
					}
					if((ucTimes>=0)&&(gBleCurrentMode==BLE_PENETRATE_TRANS_MODE))
					{
						gBleConnectStatus=BLE_CONNECTED;
						BlePrintf("ble connected\r\n");
						break;
					}
				} 
			}
			BlePrintf("ble waiting for connecting\r\n");
			BleDelay_ms(1000); 
		} 
		BlePrintf("ble no connecting\r\n");
	}
	else
	{
		gBleConnectStatus=SET;               //已连接直接设置标志
		BlePrintf("ble already connected\r\n");
	}
}
/**
  * @brief  蓝牙串口初始化 
  * @param  Baud: 设置串口波特率  
  */
static void BleUartInit(unsigned int Baud )
{ 
	
	gBleUartHandle.Instance= UART5;
	gBleUartHandle.Init.BaudRate=Baud;
	gBleUartHandle.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	gBleUartHandle.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	gBleUartHandle.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位 
	gBleUartHandle.Init.Mode=UART_MODE_TX_RX;		    //
	HAL_UART_Init( (UART_HandleTypeDef*)&gBleUartHandle); 
	HAL_UART_Receive_IT(&gstUart5.UartHandle,(uint8_t *)&gstUart5.ucRecByte,1); 
	//BleRxByteITSet(gBleRxByte);
}
/**
  * @brief  蓝牙串口IO初始化   
  */
static void BluetoothGPIOInit(void )
{
	GPIO_InitTypeDef GPIO_Initure;
	BL_MRDY_RCC();           		//开启GPIOB时钟
	BL_CONNECTION_RCC()	;
	//DIO_6 输入/输出 MRDY 引脚，用于触发模块的串口接收功能（低电平接收）
	GPIO_Initure.Pin=BL_MRDY_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	
	HAL_GPIO_Init(BL_MRDY_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET);	
	//DIO_8 输入/输出 连接指示，当模块建立连接后输出低电平
	GPIO_Initure.Pin=BL_CONNECTION_PIN; 		//
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//输入模式
	HAL_GPIO_Init(BL_CONNECTION_GPIO,&GPIO_Initure);
	//nRESET 输入 复位脚，内部有上拉（详见 CC2650 手册）
	BL_RESET_RCC();
	GPIO_Initure.Pin=BL_RESET_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	
	HAL_GPIO_Init(BL_RESET_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_SET); 
}
/**
  * @brief  蓝牙硬件复位
  */
static void BleReset(void)
{
	BL_RESET_PIN_RESET();
	BleDelay_ms(20);
	BL_RESET_PIN_SET();
}


/**
  * @brief  设置蓝牙波特率
  * @note   查询是否为设定波特率，不是则设置是则返回成功
  * @param  Baud: 用户实际地址
  * @retval 错误代码 eBleErrorType
  */
#define BAUD_STR_MAX       10
static eBleErrorType BleBaudSet(unsigned int Baud)
{ 
	eBleErrorType res; 
	char temp[BAUD_STR_MAX]={0};  
	unsigned char ucTimes=BLE_CMD_TRY_TIMES;
	if((gBleBaud!=BLE_DEFAULT_BAUD)&&(gBleBaud!=BLE_PENETRATE_MODE_BAUD))//读出来的波特率不正确
	{
		gBleBaud=BLE_DEFAULT_BAUD;//设置为蓝牙默认波特率
	}	
	BleUartInit(gBleBaud);                    //串口波特率设置为上次存入的波特率
	if(gBleBaud==BLE_PENETRATE_MODE_BAUD)      //已经设置成透传波特率
	{
		while(ucTimes--)                          //退出透传进入命令模式
		{
			res=BleToothSendCmd(BL_MODE,BLE_CMD_SET,NULL); 
			if(res==BLE_NO_ERR)
			{
				if(gBleCurrentMode==BLE_PENETRATE_TRANS_MODE)       //设置成功
					break;
			}
		}
	}
	else
	{
		while(ucTimes--)                          //退出透传进入命令模式
		{
			res=BleToothSendCmd(BL_MODE,BLE_CMD_SET,NULL); 
			if(res==BLE_NO_ERR)
			{
				if(gBleCurrentMode==BLE_CMD_MODE)       //设置成功
					break;
			}
		}
		if(ucTimes==0) // 退出分析
		{
			if(res!=BLE_NO_ERR) 
			{
				BlePrintf("ble set baud :baud err\r\n");
				return BLE_BAUD_ERR;
			}
			else if(gBleCurrentMode!=BLE_CMD_MODE)
			{
				BlePrintf("ble set baud :try err\r\n");
				return BLE_TRY_ERR;//未设置为命令模式 返回尝试设置失败
			}
		}   
		res=BleToothSendCmd(BL_UART,BLE_CMD_READ,NULL);
		if(res==BLE_NO_ERR)
		{ 
			if(gBleBaud==Baud)
			{
				return BLE_NO_ERR;
			}
			else 
			{
				sprintf(temp,"%d",Baud);
				res=BleToothSendCmd(BL_UART,BLE_CMD_SET,temp);
				if(res==BLE_NO_ERR) 
				{
					BlePrintf("ble set baud ok\r\n");
					BleUartInit(Baud);  //当前波特率
				}
				res=BleToothSendCmd(BL_UART,BLE_CMD_READ,NULL); 
				if(gBleBaud==Baud)
				{
					SaveSysInfo();
					return BLE_NO_ERR;
				}
				else 
				{
					BlePrintf("ble set baud :err=%d\r\n",BLE_BAUD_ERR);
					return BLE_BAUD_ERR;
				}
			} 
		}
		else 
		{ 
			BlePrintf("ble set baud :err=%d\r\n",res);
			return res;
		}	
	}		
} 

static void BleAnalysisFireInfo(unsigned char *pData)
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
#define TEMP_BUF_MAX   1024
static void BleGetFireInfo(void)
{
	unsigned short len =0;
	char * pdata=malloc(TEMP_BUF_MAX),*pSendStr=malloc(TEMP_BUF_MAX);
	if(pdata==NULL)     while(1);
	if(pSendStr==NULL)  while(1);
	memset(pdata,0,TEMP_BUF_MAX);  
	sprintf(pdata,"{\"Version\":[{\"ModuleName\":\"%s\",\"HardVersion\":\"%s\",\"FireVersion\":\"%s\",\"DataTime\":\"%s\"}]}"\
	,gstUpdate.stFireInfo.ModuleName\
	,gstUpdate.stFireInfo.HardVersion\
	,gstUpdate.stFireInfo.FireVersion\
	,gstUpdate.stFireInfo.Time);  
	BleSend2Phone(pdata); 
	free(pSendStr);
	free(pdata);
	pSendStr=NULL;
	pdata=NULL;
} 
static void BleUpdateApp(void)            
{
	char *pData=NULL;
	unsigned int unBinDatalen=0;
	BleAnalysisFireInfo(gstBleData.ucData); 
	BleDelay_ms(500);
 	BLE_RX_BYTE_FLAG=RESET;
	BleCmdNormalRxMode_Enter();   //蓝牙串口进入normal接收状态
	BLE_RX_BYTE_FLAG=RESET;
	gstUpdate.unCurrentAppSize= Ymodem_Receive(gucAppBuf);      //ymodem接收 bin文件
	BleCmdNormalRxMode_Exit(); //蓝牙串口退出normal接收状态
	gstUpdate.unCurrentAppADDR=(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)?USER2_APP_ADDR:USER1_APP_ADDR;
	pData=pGetAppBin((char *)gucAppBuf,gstUpdate.unCurrentAppSize,gstUpdate.unCurrentAppADDR); //取bin文件位置指针
	if(pData==NULL )
	{		
		BlePrintf("ble update addr err");
		while(1);
	}
	unBinDatalen=(*(unsigned int *)pData); //取bin文件长度
	gstUpdate.unCurrentAppSize=unBinDatalen;
	pData+=4;//偏移到bin文件开始位置
    if(CheckAppBin((unsigned int *)pData)!=0)//检测APPbin文件
	{
		BlePrintf("ble update AppBin err");
		while(1);
	}
	SaveAppBackup(pData,unBinDatalen);             //APP备份
	if(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)      //判断位置将相应状态记录
	{
		gstUpdate.eAppUpdateStatus=USR1_START_UPDATE;
	}
	else if(gstUpdate.unCurrentAppADDR==USER2_APP_ADDR)
	{
		gstUpdate.eAppUpdateStatus=USR2_START_UPDATE;
	}
	else 
	{
		BlePrintf("ble update current adr err");
		while(1);
	}
	SaveSysInfo();//存储状态信息
	InterFlash_Program(gstUpdate.unCurrentAppADDR,pData,gstUpdate.unCurrentAppSize, FLASH_TYPEPROGRAM_BYTE);//写入APP
	RunAPP(gstUpdate.unCurrentAppADDR);           //跳转APP
}
/*
char :headpos 0
char :cmd     1
unsigned short: len
char *:data
char :tailpos
 //  datalen : len-cmdlen
*/
static void BleAnalysisData(char *pData,unsigned int Len)
{
	gstBleData.ucCmd=pData[3];                          //接收命令
	gstBleData.usDataLen=(pData[1]<<8)+pData[2]-1;      //数据长度
	if(gstBleData.usDataLen<BLE_DATA_BUF_MAX)               //数据小于buf大小
	memcpy(gstBleData.ucData,(void*)&pData[4],gstBleData.usDataLen);//拷贝数据到buf
} 
#define SEND_2PHONE_BUF_MAX   1024

void str_esc(char *pReturn,char*pSource,char target1,char target2)
{
	while(*pSource!='\0')
	{
		if(*pSource++==target1)
		{
			*pReturn++=ESCAPE_CHAR;
			*pReturn++=(*pSource)^XOR_CHAR;
		}
		else if(*pSource++==target2)
		{
			*pReturn++=ESCAPE_CHAR;
			*pReturn++=(*pSource)^XOR_CHAR;
		}
		else if(*pSource++==ESCAPE_CHAR)
		{
			*pReturn++=ESCAPE_CHAR;
			*pReturn++=(*pSource)^XOR_CHAR;
		}
		else 
		{
			*pReturn++=*pSource++;
		}
	}
}

/**
  * @brief  发送数据到手机蓝牙
  * @note   数据进行转义校验加帧头尾进行发送
  * @param  pData: 发送字符串
  */
void BleSend2Phone(char *pData)
{ 
	char cDataBuf[SEND_2PHONE_BUF_MAX]={0};
	unsigned short len =0,crc16=0;
	char cLen[3]={0};  
	len=strlen(pData);                     //获取长度
	crc16=bt_crc16((uint8_t*)pData,len);   //计算校验
	str_esc(cDataBuf,pData,BLE_FRAME_HEAD_STR,BLE_FRAME_TAIL_STR); //对数据中和帧尾字符相同的进行转义
	strcpy(pData,cDataBuf);                 //更新数据
	memset(cDataBuf,0,SEND_2PHONE_BUF_MAX); 
	cLen[0]=len&0x00ff;          //取低字节
	cLen[1]=len>>8;          //取高字节
	memset(cLen,0,3);
	cLen[0]=BLE_FRAME_HEAD_STR ;//取头
	strcat(cDataBuf,cLen);       //拼接头
	strcat(cDataBuf,cLen);       //拼len字节 
	strcat(cDataBuf,pData);      //拼接数据 
	cLen[0]=crc16&0x00ff;
	cLen[1]=crc16>>8;
	strcat(cDataBuf,cLen);      //拼接校验
	memset(cLen,0,3);
	cLen[0]=BLE_FRAME_TAIL_STR ;//取尾
	strcat(cDataBuf,cLen);      //拼接帧尾
	BleSendStr(cDataBuf);//发送
}




