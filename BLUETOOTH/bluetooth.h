#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__
#include "uart5.h"
#include "rtk.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#define BLE_BOUND 		"9600"
#define BLE_UART_BOUND	9600

#define BLE_DEVELOPMENT_BOARD	0  //==1�ǿ�����

/*
�������յĴ������ȫȫ�ֱ������� 
*/
#define BLUETOOTH_RX_STATUS 	gUSART5_RX_STATUS
#define BLUETOOTH_UART_NUM 		gUART5_NUM
#define BLUETOOTH_RX_BUF 		gUSART5_RX
#define BLUETOOTH_REC_LEN		USART5_REC_LEN
#define MAX_BL_COMMAND_ECHO		100  //����������Ļ������ݴ�С char

#define BlSendCommand(command)			  UART5_SendBytes(command) //������������
#define BlSendData(data)			UART5_SendBytesSizeof(data)//�������ݽṹ
#define BlSendDataLen(data,len)	 	UART5_SendDataBytes(data,len)//����ָ�����ȵ��ڴ�
#define BlSendByte(ch)	 			UART5_SendByte(ch)//����һ���ַ�


/*
������������ſ��ƺ�״̬��ص����ų�ʼ��
*/
#if BLE_DEVELOPMENT_BOARD
	/*����DIO_6���ſ��Ƶ��������*/
	#define	BL_MRDY_PIN				GPIO_PIN_0
	#define	BL_MRDY_GPIO			GPIOB
	#define BL_MRDY_RCC()			__HAL_RCC_GPIOB_CLK_ENABLE()
	//DIO_6:״̬�������
	#define	BL_MRDY_PIN_SET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_SET)
	#define	BL_MRDY_PIN_RESET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET)

	/*����ָʾ����ģ�齨�����Ӻ�����͵�ƽ ����DIO_8���ſ��Ƶ��������*/
	#define	BL_CONNECTION_PIN			GPIO_PIN_1
	#define	BL_CONNECTION_GPIO			GPIOB
	#define BL_CONNECTION_RCC()			__HAL_RCC_GPIOB_CLK_ENABLE()
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
#else
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
#endif

/*
���������ݸ�ʽ
����ͷ��$
���ݳ��ȣ�u16����ʾ���ݵĳ���
�������ͣ�u8����ʾ��ʲô���ݣ���Ԥ����
���ݣ�����
����β@

*/
extern char gBlRtcmOpenFlag;
#define BLUETOOTH_DATA_HEAD  '#'
#define BLUETOOTH_DATA_END   '@'
#define  ESCAPE_CHARACTER	0x7d //ת���ַ�
#define  XOR_VALUE			0x20	//���ֵ
/*
������������ݶ���
*/
/*
data_types;	//��������

*/
#pragma pack(1)  

typedef  struct 
{
	u16	data_len;//���ݳ���
	u8	data_types;	//��������
	u8  *data;//����

}stBluetoothData;
#pragma pack()
/*
����״̬��¼
*/
//����������״̬��¼
enum  eBlConnection
{
	eBlConnectionY=0,  //�Ѿ�����  
	eBlConnectionN,
};
//����������ģʽ��¼ ��������������ģʽ��
//ģʽ�ش���ʾ�ַ�
#define COMMAND_MODE_RETURN		"CMD IN"
#define PENETRATE_MODE_RETURN	"CMD OUT"
enum  eBlMode
{
	eBlUCommandMode=0,  //�Ѿ�����ģʽ 
	eBlUPenetrateMode,	//��͸
	eBlUModeNC,		
	eBlUModeN,	       //û��ȷ����������ʲôģʽ��
};
//��÷�������Ļ���
enum  eBlCommandEcho
{
	eBlCommandEchoY=0,  //��ȡ������Գɹ�
	eBlCommandEchoN,

};
//��ǻ������е�������û�н�����
enum  eBlRxBuffAnalysis
{
	eBlRxBuffAnalysisY=0,  //���ݽ������
	eBlRxBuffAnalysisIng,
	eBlRxBuffAnalysisN,
	
};
//�����Ƿ��ͳɹ�
enum  eBlCommandSendStatus
{
	eBlCommandSendY=0,  //�ɹ�
	eBlCommandSendN,
	
};
//����״̬���ݶ���
typedef  struct 
{
	uint32_t connection:1;//����������״̬��¼	eBlConnection
	uint32_t mode:2;	//����������ģʽ��¼	eBlMode
	uint32_t command_echo:1;//��������Ļ���	eBlCommandEcho
	uint32_t rx_buff_analysis:2;//��ǻ������е�������û�н�����	eBlRxBuffAnalysis
	uint32_t command_send_status:1;//�����Ƿ��ͳɹ�
	uint32_t	:25;
}stBluetoothStatusBit;

typedef union{
	uint32_t				data;
	stBluetoothStatusBit	bit;
}stBlStatus;
/*
����״̬��¼end
*/
/*
�������ֻ������ݣ�(������)����ģʽ���������ͣ�
1���豸���ͣ��ֳ֡���վ���豸�����ͣ���̨�豸��ʲô�豸�����豸�ı�ţ���̨�豸�ǵ�	����ͬ���͵��豸��
2��GGA���ݣ������ֻ���ΪRTCM���ݵķ�����
3����̨��������ã����ŵ��Ķ�ȡ��
4��ϵͳ�Ĺ���״̬������Ϊϵͳ�ļ��:���ֻ��˼��(�ŵ��������豸ID��)��

�ֻ������������ݣ���д���ݣ�����ģʽ���������ͣ�
1����̨��������ã����ŵ����ã�
2����վ�ֳ���ģʽ�л�
3��RTCM����
*/
//(������)����ģʽ����������
typedef enum  eBlRequestDataTypes//�������������
{
	eBlDataRequestTypes_DeviceId=0,	//�����豸ID��
	eBlDataRequestTypes_GGA,//gga����
	eBlDataRequestTypes_Channel,//�ŵ�
	eBlDataRequestTypes_SysStatus,//ϵͳ״̬
	eBlDataRequestTypes_BleSetStatus,//�����������������Ƿ�ɹ�
}eBlRequestDataTypes;
//��д���ݣ�����ģʽ����������
typedef enum  eBSetUpDataTypes//���õ���������
{

	eBlDataSetUpTypes_Channel=0,//��̨��������ã����ŵ����ã�
	eBlDataSetUpTypes_ModeSwitch,//��վ�ֳ���ģʽ�л�
	eBlDataSetUpTypes_Rtcm,	//RTCM����
	eBlDataSetUpTypes_SelfCalibration,	//RTK��У׼
	eBlDataSetUpTypes_SetPosition,		//�ֶ����û�վ�����Ϣ
}eBlSetUpDataTypes;
#define BlDataTypes(x)	(0x1<<x)//��� eBSetUpDataTypes   eBlRequestDataTypes�����Ӧ�Ĳ�ͬ��λ��Ӧ��ͬ������

typedef enum  eBlDataMode
{
	eBlDataRequestMode=0,	//����ģʽ
	eBlDataSetUpMode,		//����ģʽ
	eBlDataSetStatusUpMode,		//����״̬����
	
}eBlDataMode;
/*
�������ͽ����������Ͷ���
*/

#pragma pack(1)
typedef  struct 
{
	uint32_t  DataMode:2;	//���ݵ�ģʽ��0:�������ݣ�1:��������,2:����������ʾ��û�н��յ�������������Ϣ��
	uint32_t  DataTypes:30;	//���ݵ����ͣ�ÿһ��bitλ��Ӧһ���������ͣ�
}stBlDataTypesBit;
typedef union{
	uint32_t			data;
	stBlDataTypesBit	bit;
}stBlDataTypes;
/*
�������ͽ����������Ͷ���end
*/
typedef  struct 
{
	stBlDataTypes	DataTypes;	//���ݵ�����
	uint32_t		DataLen;	//���ݵĳ���(ԭʼ���ݳ���)
	uint32_t		DataEscapedLen;	//���ݵĳ���(ԭʼ���ݳ���)
}stBlDataHead;
#pragma pack() 

/*
�������Դ���
*/
extern  void test_bluetooth(void);
//�������ݽ���
extern  void BluetoothAnalysis(char **data_return,stBlStatus *BlStatus);
//��ʼ��������ص�IO
extern  void BluetoothGPIO(void );
//������ݻ���ռ�
extern  void BlRxDatabuffClear(void);
/*
����ģʽ�µ�����ش����ݻ�ȡ
*/
extern  void BlCommandReturnAnalysis(char *return_data,stBlStatus *BlStatus);
/*
��������Ļ������ݻ���ռ�
*/
extern	void  BlCommandEchoBuffClear(void);
/*
�л���͸��ģʽ(��������ģʽ)
*/
extern	void BlUPenetrateMode(stBlStatus  *status);
/*
�л�������ģʽ(��������ģʽ)
*/
extern	void BlUCommandMode(stBlStatus  *status);
/*
	��ʼ�������ȴ���������
	�ȴ��豸����
	�л�������͸��ģʽ
*/
extern	void BlInit(stBlStatus  *status);
/*
�����й����ж����ˣ��ȴ����½�������
*/
extern  void BlReconnection(stBlStatus  *status);
/*
Ӳ����λ
*/
extern void BlReset(void);

/*
����RTCM����
*/

extern void BlRTCMSet(char *src_data,stBlDataHead *head,stBlStatus *BlStatus);
extern void BlReceiveDataUnpacking( char** data,stBlStatus *status,stRtkStatus *RtkStatus);
extern void BlRequest_GGA(stBlDataHead *BlDataHead,stRtkStatus	*RtkStatus );
/*
ȫ�ֱ�������
*/
extern	char		gCommandEchoBuff[MAX_BL_COMMAND_ECHO];//�洢������Ե�buff
extern	stBlStatus  gBlStatus;//������״̬
/*
���Դ���
*/
extern void BlTest(void);
/*
����ģ����Ժ���
*/
void BlTest(void);
#endif

