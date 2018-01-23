#ifndef __RADIO_H_
#define __RADIO_H_
#include "sys.h"
#include "uart7.h"

#define	RadioSendByteS(X)				UART7_SendBytes(X)	//���Ͷ���ֽڵ�����  ��'\0'����
#define	RadioSendByteSizeof(X) 			UART7_SendBytesSizeof(X)//����һ���ڴ�
#define	RadioSendByte(X)				UART7_SendByte(X)	//����һ���ֽ�
#define	RadioSendByteSLen(data ,len )	UART7_SendBytesLen(data ,len)	//����һ���ֽ�

#define	RadioUartBound(X)				UART7_Bound(X)		//���õ�̨��Ӧ�Ĵ��ڵĲ�����
#define	RADIO_RETURN_PARAMETER_CMD	0xc1c1c1//�����ѱ���Ĳ���  
#define	RADIO_RETURN_VERSION_CMD	0xc3c3c3//ģ�鷵�ذ汾��Ϣ
#define	RADIO_RETURN_RESET_CMD		0xc4c4c4//ģ�齫����һ�θ�λ 
//�㲥ģʽ�ĵ�ַ����
#define	RADIO_BROADCAST_MODE_ADD_L	0xFF
#define	RADIO_BROADCAST_MODE_ADD_H	0xFF

#define	RADIO_DATA_HADE_LEN	3


/*
�����������
*/
//�����Ƿ���籣��
typedef enum	eRadioParameterSave
{
	eRadioParameterSaveY=0xc0,// C0�������õĲ�������籣��
	eRadioParameterSaveN=0xc2,//	C2�������õĲ���������籣�档
}eRadioParameterSave ;
//����У��λ 
typedef enum	eRadioCmdSerialCheck
{
	eRadioSerialCheck_8N1=0,//Ĭ��
	eRadioSerialCheck_8O1,
	eRadioSerialCheck_8E1,
}eRadioCmdSerialCheck;
//���ڲ�����
typedef enum	eRadioCmdSerialRate
{
	eRadioSerialRate_1200 =0,
	eRadioSerialRate_2400,
	eRadioSerialRate_4800,
	eRadioSerialRate_9600,//Ĭ��
	eRadioSerialRate_19200 ,
	eRadioSerialRate_38400 ,
	eRadioSerialRate_57600 ,
	eRadioSerialRate_115200,

}eRadioCmdSerialRate;
//���ٶ�
typedef enum	eRadioCmdAirRate
{
	eRadioAirRate_2_5k=0,//Ĭ��
	eRadioAirRate_5k,
	eRadioAirRate_12k,
	eRadioAirRate_28k ,
	eRadioAirRate_64k ,
	eRadioAirRate_168k,
}eRadioCmdAirRate;
//ͨ���ŵ�
#define  MAX_RADIO_CHANNEL 0x1f//(���Ŀ��)��Ĭ�� 02H��433MHz��ͨ���ŵ�����431M + CHAN*1M�� Ĭ�� 02H��433MHz��00H-1FH�� �� Ӧ 431~462MHz  
//�����趨 ����λ�ֽڣ�
typedef enum	eRadioPackageLengthSetting 
{
	eRadioPackageLength_16=0,//
	eRadioPackageLength_32,
	eRadioPackageLength_64,//Ĭ��
	eRadioPackageLength_128,
	eRadioPackageLength_256,
	eRadioPackageLength_512,
	eRadioPackageLength_1024,
	eRadioPackageLength_2048,
}eRadioPackageLengthSetting;
//���书��(��Լֵ) 
typedef enum	eRadioTransmitPower 
{
	eRadioTransmitPower_30dbm=0,//Ĭ��
	eRadioTransmitPower_27dbm,
	eRadioTransmitPower_24dbm,
	eRadioTransmitPower_17dbm,
}eRadioTransmitPower;
// ������ʽ��Ĭ��1�� 
typedef enum	eRadioIODriveMode 
{
	eRadioIODriveMode_OpenCircuit=0,// 0��TXD��AUX��·�����RXD��·����
	eRadioIODriveMode_PushPullUp,// 1��TXD��AUX���������RXD�������� ��Ĭ��1�� 

}eRadioIODriveMode;
// FEC ����  
typedef enum	eRadioFECSwitch 
{
	eRadioFECSwitch_Close=0,// �ر� FEC ����
	eRadioFECSwitch_Open,// �� FEC ����Ĭ�ϣ� 

}eRadioFECSwitch;
// ����ʱ��  
typedef enum	eRadioWakeUpTime 
{
	eRadioWakeUpTime_500ms=0,
	eRadioWakeUpTime_1000ms,//Ĭ��
	eRadioWakeUpTime_1500ms,
	eRadioWakeUpTime_2000ms,
	eRadioWakeUpTime_2500ms,
	eRadioWakeUpTime_3000ms,
	eRadioWakeUpTime_3500ms,
	eRadioWakeUpTime_4000ms,
}eRadioWakeUpTime;
// ���㷢��ʹ��λ  
typedef enum	eRadioSentToSent
{
	eRadioSentToSent_Transparent=0,//͸������ģʽ��Ĭ�ϣ�
	eRadioSentToSent_FixedPoint,// ���㴫��ģʽ 

}eRadioSentToSent;
#pragma pack(1)  
/*
���ʲ����������������ʺͿ������� 
*/
typedef	union{
	struct{
		u8	AirRate:3;//��������
		u8	SerialRate:3;//��������
		u8	SerialCheck :2;//����У��λ 
	}bit;
	u8 byte;
}stRadioSped;
/*
�����趨�������ڷְ�ģʽ�� 
ͨ���ŵ�����431M + CHAN*1M�� 
*/
typedef	union{
	struct{
		u8	CommunicationChannel :5;//ͨ���ŵ�  00H-1FH
		u8	PackageLengthSetting :3;//�����趨
	}bit;
	u8 byte;
}stRadioChan ;
/*
7�����㷢��ʹ��λ���� MODBUS�� 
6 , 5 , 4   ����ʱ�� 
3,        FEC ����       
2       IO ������ʽ��Ĭ��1��
1, 0     ���书��(��Լֵ) 
*/
typedef	union{
	struct{
		u8	TransmitPower	:2;//���书��(��Լֵ) 
		u8	IODriveMode		:1;// ������ʽ��Ĭ��1��
		u8	FECSwitch		:1;// FEC ���� 
		u8	WakeUpTime		:3;//����ʱ�� 
		u8	SentToSent		:1;//���㷢��ʹ��λ
	}bit;
	u8 byte;
}stRadioOption  ;
/*
��̨��������
*/
typedef	struct{
	u8	head;//eRadioCmdHead C0�������õĲ�������籣�档 C2�������õĲ���������籣�档
	u8	addH;//ģ���ַ���ֽڣ�Ĭ��00H��
	u8	addL;//ģ���ַ���ֽڣ�Ĭ��00H�� 
	//SPED 
//	u8	AirRate:3;			//��������
//	u8	SerialRate:3;		//��������
//	u8	SerialCheck :2;		//����У��λ 
	stRadioSped sped;
	//CHAN 
//	u8	CommunicationChannel :5;	//ͨ���ŵ�  00H-1FH
//	u8	PackageLengthSetting :3;	//�����趨
	stRadioChan chan;
	//OPTION 
//	u8	TransmitPower:2;	//���书��(��Լֵ) 
//	u8	IODriveMode:1;		// ������ʽ��Ĭ��1��
//	u8	FECSwitch:1;		// FEC ���� 
//	u8	WakeUpTime:3;		//����ʱ�� 
//	u8	SentToSent:1;		//���㷢��ʹ��λ
	stRadioOption option; 
}stRadioCommandParameter;
#pragma pack()  
/*
��̨�Ĺ���ģʽ
*/
typedef enum  eRadioWorkMode
{
	eRaRssiMode=0,					//0 RSSI ģʽ	���ڼ����������� 
	eRaContinuousTransmissionMode,	//1 ����ģʽ	˫���Ĵ��ڲ����趨����һ��
	eRaSubcontractMode,				//2 �ְ�ģʽ	�÷�ʽ�£����ڲ����ʺͿ����Ƕ����ģ��շ�˫�����Ծ��в�ͬ�Ĵ��ڲ����ʣ������������ͬ�Ŀ����趨�� 
	eRaConfigurationMode,			//3 ����ģʽ	��������ģ��������ã�ʹ�ô��� 9600��8N1��ͨ���ض�ָ���ʽ����ģ�鹤������ 
	eRaWakeMode,					//4 ����ģʽ 
	eRaPowerSavingMode=6,			//6 ʡ��ģʽ 
	eRaSleepMode,					//7 ����ģʽ 
}eRadioWorkMode;
/*��̨������״̬*/
typedef enum	eRadioReady
{
	eRadioReadyY=0, //�豸׼������
	eRadioReadyN,	
}eRadioReady ;
/*��̨�ŵ�������״̬��¼*/
typedef enum	eRadioChannel
{
	eRadioChannelY=0, //��̨�ŵ�������״̬��¼�ɹ�
	eRadioChannelN,	
}eRadioChannel;
/*��̨��״̬��Ϣ */
typedef  struct 
{
	uint32_t 	ready:1;//��̨״̬׼��״̬  eRadioReady
	uint32_t	mode:3;//��¼��ǰ�Ĺ���ģʽ	eRadioWorkMode
	uint32_t	channel:1;//�����ŵ���״̬��¼ 
	uint32_t	:32-5;
}stRadioStatusBit;
typedef union{
	uint32_t			data;
	
	stRadioStatusBit	bit;
}stRadioStatus;

/*��̨������״̬ end*/


/*
��̨����״̬���
status:���ص�̨���豸״̬
eRadioReady��
*/
extern eRadioReady RaReadAux(stRadioStatus *status);
/*
ģʽ�л�����
*/
extern void RaModeSwitch(eRadioWorkMode eMode,stRadioStatus *status);
/*
	���ڲ����ʺ͵�̨������ƥ��
*/
extern void RadioUartBoundMatch(eRadioCmdSerialRate SerialRate);
/*
�л�������ģʽ��ģʽ 3�� :�л�������ģʽ��ʼ���ö�Ӧ�Ĳ�������
*/
extern void RadioConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud );
/*
	�˳�����ģʽ
	�жϲ�������û�иı�
baud: �������ݵĲ�����
*/
extern void RadioExitConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud);

/*
���͵�̨���������õ�̨
*/
extern void RadioParameterSend(stRadioCommandParameter *parameter,	stRadioStatus *status);

/*
��ʼ����̨
*/

extern void RadioInit(stRadioStatus *status,stRadioCommandParameter *parameter);
/*
���ŵ�
*/
uint8_t RadioChannelRead(stRadioCommandParameter *parameter);
/*
	��̨���ݷ��ͣ�ͨ����̨�������ݷ��͸�����������豸
	�����͵�������GGA��RTCM����
*/
extern void RadioDataSend(stRadioCommandParameter *parameter,char *data);
/*
��̨���ŵ�����
��������32���ŵ�
Ĭ�� 02H��433MHz��ͨ���ŵ�����431M + CHAN*1M�� Ĭ�� 02H��433MHz��00H-1FH�� �� Ӧ 431~462MHz 
channel  �ŵ�ֵ(���ֵ��0��31 )
parameter �����Ѿ����úõĲ���
status ��״̬����
*/
extern void RadioChannelSet(uint8_t channel,stRadioCommandParameter *parameter,stRadioStatus *status);
/*
���Դ���
*/
extern void radio_test(void);




extern stRadioStatus gRadioStatus;//״̬�ļ�¼
extern stRadioCommandParameter gRadioCommandParameter;//������¼

#endif








