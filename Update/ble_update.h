

#ifndef BLE_UPDATE_H
#define BLE_UPDATE_H
#include "stm32f7xx.h"
#include "uart.h"
#define  BlSendCommand          UART2_SendString

#define BLUE_DEBUG  0

#if     BLUE_DEBUG        
#define  BlePrintf         DBG_Printf
#else   
#define  BlePrintf         print_null
#endif

/*����DIO_6���ſ��Ƶ��������  M_BL_MRDY*/
#define	BL_MRDY_PIN				GPIO_PIN_4
#define	BL_MRDY_GPIO			GPIOK
#define BL_MRDY_RCC()			__HAL_RCC_GPIOK_CLK_ENABLE()
//DIO_6:״̬�������
#define	BL_MRDY_PIN_SET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_SET)
#define	BL_MRDY_PIN_RESET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET)

/*����ָʾ����ģ�齨�����Ӻ�����͵�ƽ ����DIO_8���ſ��Ƶ��������  M_BL_LINK*/
#define	BL_CONNECTION_PIN			GPIO_PIN_6
#define	BL_CONNECTION_GPIO			GPIOK
#define BL_CONNECTION_RCC()			__HAL_RCC_GPIOK_CLK_ENABLE()
//DIO_8:״̬��ȡ
#define BL_CONNECTION_STATUS()		HAL_GPIO_ReadPin(BL_CONNECTION_GPIO,BL_CONNECTION_PIN) //0��ʾ�������� 1�Ͽ�
/*
��λ���������Ϣ
*/
#define	BL_RESET_PIN			GPIO_PIN_5
#define	BL_RESET_GPIO			GPIOB
#define BL_RESET_RCC()			__HAL_RCC_GPIOB_CLK_ENABLE()
//��λ���ŵ�����
#define	BL_RESET_PIN_SET()		HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_SET)
#define	BL_RESET_PIN_RESET()	HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_RESET)


#define BLE_DATA_BUF_MAX     1024


typedef enum ble_update_status
{
	BLE_START_UPDATE,
	BLE_CONNECTED,
}eBleUpdateStaType;

typedef struct ble_rx_data_st
{
	unsigned char  ucCmd;
	unsigned short usDataLen;
	unsigned char  ucData[BLE_DATA_BUF_MAX];
}stBleDataType; 

typedef struct ble_info
{
	unsigned  int  unBaud;
	unsigned  char ucUpdateStatus;
	unsigned  char ucConnectStatus;
    unsigned  char ucBleMode;
}stBleInfoType ;
typedef enum 
{
	BLE_CMD_UPDATE='U',
	BLE_CMD_GET_INFO='G'
}eBleUpdateCmdType;

//�������յ����� �ṹ��Աλ��ƫ��
#define MODULE_NAME_POS  27
#define HARD_VERSION_POS  17
#define FIRE_VERSION_POS  17
#define DATA_TIME_POS     14
#define END_BYTE_1       ','
#define END_BYTE_2       '"'
#define END_BYTE_3       '}'
#define END_BYTE_4       ','
#define END_BYTE_5       ','
//�����շ�����
#define BLE_FRAME_HEAD_STR    UART5_HEAD_CHAR
#define BLE_FRAME_TAIL_STR    UART5_TAIL_CHAR





//����buf
#define   gBleRxBuffer                gstUart5.Buffer                     //����buffer
#define   gBleRxBufferLen             gstUart5.unBufferLen	       //���ճ���
#define   gBleRxByte                  gstUart5.ucRecByte         
//������־
#define   BLE_RX_FRAME_FLAG           UART5_RX_GOT_FRAME_FLAG                 //����֡��ɱ�־
#define   BLE_RX_BYTE_FLAG            UART5_RX_GOT_BYTE_FLAG                   //�����ֽ���ɱ�־
#define   BLE_UPDATE_START            gstUpdate.stBleInfo.ucUpdateStatus
//��������
#define   gBleBaud                    gstUpdate.stBleInfo.unBaud
#define   gBleCurrentMode             gstUpdate.stBleInfo.ucBleMode
#define   gBleConnectStatus           gstUpdate.stBleInfo.ucConnectStatus
extern    stBleDataType  gstBleData;

 

//�������ƾ��
#define gBleUartHandle           gstUart5.UartHandle  
//�����շ�����
#define BleSendBytes             COM5_SendBytes 
#define BleRecBytes(pdata,size)             do{COM5_RecBytes(pdata,size) ; while(HAL_UART_GetState((UART_HandleTypeDef*)&gLocalUartHandle)!=HAL_UART_STATE_READY ){}}while(0)
#define BleSendStr               UART5_SendString

#define BleDelay_ms              delay_ms
#define BleCmdNormalRxMode_Enter()               UART_RxNormalMode_Enter(gstUart5)         
#define BleCmdNormalRxMode_Exit()                UART_RxNormalMode_Exit(gstUart5)
#define BleRxBufferClear()                       UART_RxBufferClear(gstUart5)
#define BleRxFlagClear()						 UART_ClearFlags(gstUart5)  
#define BleRxBufferFindStr(pStr)            	 findstr(gBleRxBuffer,gBleRxBufferLen,pStr)
#define BleRxByteITSet(byte)					 COM5_RecBytes((uint8_t *)&(byte),1)
#define BleReceviceByte(byte,timeout)            ReceviceByte(&gstUart5,byte,timeout)
#define BleSendByte(byte)                        COM5_SendBytes((uint8_t *)&byte,1);
//��������
/**
  * @brief  ��������������
  * @note   ��ѯ�Ƿ�Ϊ�趨�����ʣ��������������򷵻سɹ�
  * @param  Baud: �û�ʵ�ʵ�ַ
  * @retval �ɹ���� 0 :�ɹ� >1 :���ɹ�
  */ 


void CreateBleTask(void);
void BleSend2Phone(char *pData);
#endif





