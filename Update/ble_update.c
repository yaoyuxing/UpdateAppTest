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




stBleDataType  gstBleData; //��������
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
	xTaskCreate((TaskFunction_t )BleTask,            //������
				(const char*    )"ble_task",         //��������
				(uint16_t       )BLE_STK_SIZE,        //�����ջ��С
				(void*          )NULL,                //���ݸ��������Ĳ���
				(UBaseType_t    )BLE_TASK_PRIO,       //�������ȼ�
				(TaskHandle_t*  )&BleTask_Handler);   //������  	 	
}
static void BleTask(void* param)
{  
	unsigned int TimeOut=0xe000000;  
	#if 0
	BleInit();    //��ʼ������ģ�� 
	#else
	BleUartInit(115200);                    //���ڲ���������Ϊ�ϴδ���Ĳ����� 
	BleSendStr("sadsad");
	gBleConnectStatus=SET;
	#endif
	while(gBleConnectStatus)       //��ʼ�����ӳɹ���ȴ��ֻ�����
	{ 
		#if 0
		if(BL_CONNECTION_STATUS())//�����й����ж�����
		{ 
			BleInit();         //��������
		} 
		TimeOut--; 
		#endif
		
		if(BLE_RX_FRAME_FLAG==SET)      //���յ�������������֡
		{ 
			BleAnalysisData((char*)gBleRxBuffer,gBleRxBufferLen);
			BLE_RX_FRAME_FLAG=RESET;
			switch(gstBleData.ucCmd)
			{
				case BLE_CMD_UPDATE:
					BleUpdateApp();    //����
					break;
				case BLE_CMD_GET_INFO:
					BleGetFireInfo(); //��ȡ��Ϣ
					break;
			}
		}
	} 
	RunAPP(gstUpdate.unCurrentAppADDR);	//û������ֱ����ת
} 
/**
  * @brief  ������ʼ��
  */
static void BleInit(void)
{
	unsigned int TimeOut=0xe000000;  
	unsigned char ucTimes=BLE_CMD_TRY_TIMES;
	int res=0; 
	BluetoothGPIOInit();
	gBleConnectStatus=RESET;
	gBleCurrentMode=BLE_CMD_MODE_NONE;
	if((gBleBaud!=BLE_DEFAULT_BAUD)&&(gBleBaud!=BLE_PENETRATE_MODE_BAUD))//�������Ĳ����ʲ���ȷ
	{
		gBleBaud=BLE_DEFAULT_BAUD;//����Ϊ����Ĭ�ϲ�����
	}	
	BleUartInit(gBleBaud);                    //���ڲ���������Ϊ�ϴδ���Ĳ����� 
	if(BL_CONNECTION_STATUS()!=0) //δ����
	{	
		BleReset();        //����  
		BL_MRDY_PIN_RESET();  //�л���RX����ģʽ
        res=BleBaudSet(BLE_PENETRATE_MODE_BAUD);    //����Ϊ͸���Ĳ�����
		if(res!=0) 
		{
			BlePrintf("ble baud set failed res =%d\r\n",res);
		} 
		//res=BleToothSendCmd(BL_NAME,BLE_CMD_READ,"yyx");
		if(res!=0) 
		{
			while(1);
		} 
		//res=BleToothSendCmd(BL_TXPWR,BLE_CMD_SET,"-3");    //���÷���ǿ��
		if(res!=0)        
		{
			BlePrintf("ble txpwr failed res =%d\r\n",res);
		}
		while(TimeOut--)          //����
		{
			if(!BL_CONNECTION_STATUS()) //���豸������
			{
				BleDelay_ms(200);
				if(!BL_CONNECTION_STATUS())//��ʱ�����ж�
				{
					while(ucTimes--)
					{
						res=BleToothSendCmd(BL_MODE,BLE_CMD_SET,NULL);       //����͸������
						if((res==BLE_NO_ERR)&&(gBleCurrentMode==BLE_PENETRATE_TRANS_MODE))//����͸��ģʽ
						{
							gBleConnectStatus=SET;           //����Ϊ͸��
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
		gBleConnectStatus=SET;               //������ֱ�����ñ�־
		BlePrintf("ble already connected\r\n");
	}
}
/**
  * @brief  �������ڳ�ʼ�� 
  * @param  Baud: ���ô��ڲ�����  
  */
static void BleUartInit(unsigned int Baud )
{ 
	
	gBleUartHandle.Instance= UART5;
	gBleUartHandle.Init.BaudRate=Baud;
	gBleUartHandle.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	gBleUartHandle.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	gBleUartHandle.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ 
	gBleUartHandle.Init.Mode=UART_MODE_TX_RX;		    //
	HAL_UART_Init( (UART_HandleTypeDef*)&gBleUartHandle); 
	HAL_UART_Receive_IT(&gstUart5.UartHandle,(uint8_t *)&gstUart5.ucRecByte,1); 
	//BleRxByteITSet(gBleRxByte);
}
/**
  * @brief  ��������IO��ʼ��   
  */
static void BluetoothGPIOInit(void )
{
	GPIO_InitTypeDef GPIO_Initure;
	BL_MRDY_RCC();           		//����GPIOBʱ��
	BL_CONNECTION_RCC()	;
	//DIO_6 ����/��� MRDY ���ţ����ڴ���ģ��Ĵ��ڽ��չ��ܣ��͵�ƽ���գ�
	GPIO_Initure.Pin=BL_MRDY_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	
	HAL_GPIO_Init(BL_MRDY_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET);	
	//DIO_8 ����/��� ����ָʾ����ģ�齨�����Ӻ�����͵�ƽ
	GPIO_Initure.Pin=BL_CONNECTION_PIN; 		//
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//����ģʽ
	HAL_GPIO_Init(BL_CONNECTION_GPIO,&GPIO_Initure);
	//nRESET ���� ��λ�ţ��ڲ������������ CC2650 �ֲᣩ
	BL_RESET_RCC();
	GPIO_Initure.Pin=BL_RESET_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	
	HAL_GPIO_Init(BL_RESET_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_SET); 
}
/**
  * @brief  ����Ӳ����λ
  */
static void BleReset(void)
{
	BL_RESET_PIN_RESET();
	BleDelay_ms(20);
	BL_RESET_PIN_SET();
}


/**
  * @brief  ��������������
  * @note   ��ѯ�Ƿ�Ϊ�趨�����ʣ��������������򷵻سɹ�
  * @param  Baud: �û�ʵ�ʵ�ַ
  * @retval ������� eBleErrorType
  */
#define BAUD_STR_MAX       10
static eBleErrorType BleBaudSet(unsigned int Baud)
{ 
	eBleErrorType res; 
	char temp[BAUD_STR_MAX]={0};  
	unsigned char ucTimes=BLE_CMD_TRY_TIMES;
	if((gBleBaud!=BLE_DEFAULT_BAUD)&&(gBleBaud!=BLE_PENETRATE_MODE_BAUD))//�������Ĳ����ʲ���ȷ
	{
		gBleBaud=BLE_DEFAULT_BAUD;//����Ϊ����Ĭ�ϲ�����
	}	
	BleUartInit(gBleBaud);                    //���ڲ���������Ϊ�ϴδ���Ĳ�����
	if(gBleBaud==BLE_PENETRATE_MODE_BAUD)      //�Ѿ����ó�͸��������
	{
		while(ucTimes--)                          //�˳�͸����������ģʽ
		{
			res=BleToothSendCmd(BL_MODE,BLE_CMD_SET,NULL); 
			if(res==BLE_NO_ERR)
			{
				if(gBleCurrentMode==BLE_PENETRATE_TRANS_MODE)       //���óɹ�
					break;
			}
		}
	}
	else
	{
		while(ucTimes--)                          //�˳�͸����������ģʽ
		{
			res=BleToothSendCmd(BL_MODE,BLE_CMD_SET,NULL); 
			if(res==BLE_NO_ERR)
			{
				if(gBleCurrentMode==BLE_CMD_MODE)       //���óɹ�
					break;
			}
		}
		if(ucTimes==0) // �˳�����
		{
			if(res!=BLE_NO_ERR) 
			{
				BlePrintf("ble set baud :baud err\r\n");
				return BLE_BAUD_ERR;
			}
			else if(gBleCurrentMode!=BLE_CMD_MODE)
			{
				BlePrintf("ble set baud :try err\r\n");
				return BLE_TRY_ERR;//δ����Ϊ����ģʽ ���س�������ʧ��
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
					BleUartInit(Baud);  //��ǰ������
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
	BleCmdNormalRxMode_Enter();   //�������ڽ���normal����״̬
	BLE_RX_BYTE_FLAG=RESET;
	gstUpdate.unCurrentAppSize= Ymodem_Receive(gucAppBuf);      //ymodem���� bin�ļ�
	BleCmdNormalRxMode_Exit(); //���������˳�normal����״̬
	gstUpdate.unCurrentAppADDR=(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)?USER2_APP_ADDR:USER1_APP_ADDR;
	pData=pGetAppBin((char *)gucAppBuf,gstUpdate.unCurrentAppSize,gstUpdate.unCurrentAppADDR); //ȡbin�ļ�λ��ָ��
	if(pData==NULL )
	{		
		BlePrintf("ble update addr err");
		while(1);
	}
	unBinDatalen=(*(unsigned int *)pData); //ȡbin�ļ�����
	gstUpdate.unCurrentAppSize=unBinDatalen;
	pData+=4;//ƫ�Ƶ�bin�ļ���ʼλ��
    if(CheckAppBin((unsigned int *)pData)!=0)//���APPbin�ļ�
	{
		BlePrintf("ble update AppBin err");
		while(1);
	}
	SaveAppBackup(pData,unBinDatalen);             //APP����
	if(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)      //�ж�λ�ý���Ӧ״̬��¼
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
	SaveSysInfo();//�洢״̬��Ϣ
	InterFlash_Program(gstUpdate.unCurrentAppADDR,pData,gstUpdate.unCurrentAppSize, FLASH_TYPEPROGRAM_BYTE);//д��APP
	RunAPP(gstUpdate.unCurrentAppADDR);           //��תAPP
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
	gstBleData.ucCmd=pData[3];                          //��������
	gstBleData.usDataLen=(pData[1]<<8)+pData[2]-1;      //���ݳ���
	if(gstBleData.usDataLen<BLE_DATA_BUF_MAX)               //����С��buf��С
	memcpy(gstBleData.ucData,(void*)&pData[4],gstBleData.usDataLen);//�������ݵ�buf
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
  * @brief  �������ݵ��ֻ�����
  * @note   ���ݽ���ת��У���֡ͷβ���з���
  * @param  pData: �����ַ���
  */
void BleSend2Phone(char *pData)
{ 
	char cDataBuf[SEND_2PHONE_BUF_MAX]={0};
	unsigned short len =0,crc16=0;
	char cLen[3]={0};  
	len=strlen(pData);                     //��ȡ����
	crc16=bt_crc16((uint8_t*)pData,len);   //����У��
	str_esc(cDataBuf,pData,BLE_FRAME_HEAD_STR,BLE_FRAME_TAIL_STR); //�������к�֡β�ַ���ͬ�Ľ���ת��
	strcpy(pData,cDataBuf);                 //��������
	memset(cDataBuf,0,SEND_2PHONE_BUF_MAX); 
	cLen[0]=len&0x00ff;          //ȡ���ֽ�
	cLen[1]=len>>8;          //ȡ���ֽ�
	memset(cLen,0,3);
	cLen[0]=BLE_FRAME_HEAD_STR ;//ȡͷ
	strcat(cDataBuf,cLen);       //ƴ��ͷ
	strcat(cDataBuf,cLen);       //ƴlen�ֽ� 
	strcat(cDataBuf,pData);      //ƴ������ 
	cLen[0]=crc16&0x00ff;
	cLen[1]=crc16>>8;
	strcat(cDataBuf,cLen);      //ƴ��У��
	memset(cLen,0,3);
	cLen[0]=BLE_FRAME_TAIL_STR ;//ȡβ
	strcat(cDataBuf,cLen);      //ƴ��֡β
	BleSendStr(cDataBuf);//����
}




