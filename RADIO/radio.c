/*
��̨��Ĭ�ϵ����ò�����
ģ���ͺ�  	  Ƶ��    ��ַ   �ŵ�  ��������  ������  ���ڸ�ʽ  ���书��
E71-433MS30  433MHz  0x0000  0x04  2.5K      9600     8N1      1W 
*/
#include "radio.h"
#include "sys.h"
#include "delay.h"
#include <stdlib.h>
#include <string.h>

stRadioStatus gRadioStatus;//״̬�ļ�¼
stRadioCommandParameter gRadioCommandParameter;//������¼
/*
��̨��IO�������
*/
//C����ص�IO����
#define	RADIO_M0_PIN	GPIO_PIN_0
#define	RADIO_M1_PIN	GPIO_PIN_1
#define	RADIO_M2_PIN	GPIO_PIN_2
#define	RADIO_AUX_PIN	GPIO_PIN_3
#define	RADIO_GPIO		GPIOC
//B����ص�IO����
#define	RADIO_PEN_PIN	GPIO_PIN_6
#define	RADIO_PEN_GPIO	GPIOB
//K����ص�IO����
#define RADIO_RESET_PIN 		GPIO_PIN_7
#define	RADIO_RESET_GPIO		GPIOK


void RadioGpioInit(void )
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOC_CLK_ENABLE();           		//����GPIOCʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();           		//����GPIOCʱ��
	__HAL_RCC_GPIOK_CLK_ENABLE();           		//����GPIOCʱ��
	//ģ���Դʹ��
	GPIO_Initure.Pin=RADIO_PEN_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
	GPIO_Initure.Pull=GPIO_PULLUP;          	//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	
	HAL_GPIO_Init(RADIO_PEN_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(RADIO_PEN_GPIO,RADIO_PEN_PIN,GPIO_PIN_SET); //��λ��������
	
	//��̨����ģʽ��������    
	GPIO_Initure.Pin=RADIO_M0_PIN|RADIO_M1_PIN|RADIO_M2_PIN;
	HAL_GPIO_Init(RADIO_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M0_PIN|RADIO_M1_PIN|RADIO_M2_PIN,GPIO_PIN_SET); //��ʼ��Ϊ����ģʽ
	
	//��̨�ķ������ݺ�����״̬�����ݷ���
	GPIO_Initure.Pin=RADIO_AUX_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//����ģʽ
	HAL_GPIO_Init(RADIO_GPIO,&GPIO_Initure);
	//ģ�鸴λ
	GPIO_Initure.Pin=RADIO_RESET_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
	GPIO_Initure.Pull=GPIO_PULLUP;          	//����
	HAL_GPIO_Init(RADIO_RESET_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(RADIO_RESET_GPIO,RADIO_RESET_PIN,GPIO_PIN_SET); //��λ��������
	
}
/*
	���ڲ����ʺ͵�̨������ƥ��
*/
void RadioUartBoundMatch(eRadioCmdSerialRate SerialRate)
{
	switch(SerialRate)
	{
		case   eRadioSerialRate_1200	: 	RadioUartBound(1200	)	;break;
		case   eRadioSerialRate_2400	: 	RadioUartBound(2400	)	;break;
		case   eRadioSerialRate_4800	: 	RadioUartBound(4800	)	;break;
		case   eRadioSerialRate_9600	: 	RadioUartBound(9600	)	;break;
		case   eRadioSerialRate_19200	: 	RadioUartBound(19200)	;break;
		case   eRadioSerialRate_38400	: 	RadioUartBound(38400)	;break;
		case   eRadioSerialRate_57600	: 	RadioUartBound(57600)	;break;
		case   eRadioSerialRate_115200	:	RadioUartBound(115200)	;break;
		default: break;
	}
}
/*
ģʽ�л�����
*/
void RaModeSwitch(eRadioWorkMode eMode,stRadioStatus *status)
{
//	while(RaReadAux(status)!=eRadioReadyY) //�ȴ��豸����
//	{
//		delay_ms(1);
//	}
	if(RaReadAux(status)==eRadioReadyY)	//�豸�����л�ģʽ
	{
		delay_ms(2);
		(eMode&0x1)?HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M0_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M0_PIN,GPIO_PIN_RESET);
		(eMode&0x2)?HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M1_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M1_PIN,GPIO_PIN_RESET);
		(eMode&0x4)?HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M2_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M2_PIN,GPIO_PIN_RESET);
		status->bit.mode=eMode;
	}
	while(RaReadAux(status)!=eRadioReadyY) //�ȴ��豸����
	{
		delay_ms(1);
	}
}
/*
��̨����״̬���
status:���ص�̨���豸״̬
eRadioReady��
*/
eRadioReady RaReadAux(stRadioStatus *status)
{
	if(HAL_GPIO_ReadPin(RADIO_GPIO,RADIO_AUX_PIN)) //0:��ʾ�豸��û��׼������  1���豸��û��׼������
	{
		status->bit.ready=eRadioReadyY;
		return eRadioReadyY;
	}else{
		status->bit.ready=eRadioReadyN;
		return eRadioReadyN;
	}
}
/*��̨���ã�����������غ���*******************************/

/*
���Ͳ�������������
*/
void RadioSendCommmandNoParameter(uint32_t data,stRadioStatus *status)
{
	u8	data_buf[4];
	RaModeSwitch(eRaConfigurationMode,status);//�л�������ģʽ
	data_buf[0]=data&0xff;
	data_buf[1]=(data&0xff)>>8;
	data_buf[2]=(data&0xff)>>16;
	data_buf[3]='\0';
	RadioSendByteS((char*)data_buf);//�������õ�����
}

/*
	��ʼ���豸����������ĵ�̨�����������������
*/
void RadioParameterInit(const u8	addH,const u8	addL,stRadioCommandParameter *parameter)
{

	parameter->head=eRadioParameterSaveY;
	parameter->addH=addH;
	parameter->addL=addL;
	//SPED
	parameter->sped.bit.AirRate=eRadioAirRate_2_5k;
	parameter->sped.bit.SerialRate=eRadioSerialRate_9600;
	parameter->sped.bit.SerialCheck=eRadioSerialCheck_8N1;
	//CHAN
	parameter->chan.bit.CommunicationChannel=0x02;
	parameter->chan.bit.PackageLengthSetting=eRadioPackageLength_64;
	//OPTION 
	parameter->option.bit.TransmitPower	=eRadioTransmitPower_30dbm;
	parameter->option.bit.IODriveMode	=eRadioIODriveMode_PushPullUp;	
	parameter->option.bit.FECSwitch		=eRadioFECSwitch_Open;
	parameter->option.bit.WakeUpTime	=eRadioWakeUpTime_1000ms;	
	parameter->option.bit.SentToSent	=eRadioSentToSent_Transparent;	
//	RadioSendByteSizeof((char*)parameter);//��������
}
/*
��̨���ŵ�����
��������32���ŵ�
Ĭ�� 02H��433MHz��ͨ���ŵ�����431M + CHAN*1M�� Ĭ�� 02H��433MHz��00H-1FH�� �� Ӧ 431~462MHz 
channel  �ŵ�ֵ(���ֵ��0��31 )
parameter �����Ѿ����úõĲ���
status ��״̬����
*/
void RadioChannelSet(uint8_t channel,stRadioCommandParameter *parameter,stRadioStatus *status)
{
	if(channel<=MAX_RADIO_CHANNEL)
	{
		parameter->chan.bit.CommunicationChannel=channel;
		status->bit.channel=eRadioChannelY;
		RadioParameterSend(parameter,status);//������Ӧ��ͨ��ֵ
	}else{
		status->bit.channel=eRadioChannelN;
	}
}
/*
���ŵ�
*/
uint8_t RadioChannelRead(stRadioCommandParameter *parameter)
{
	return parameter->chan.bit.CommunicationChannel;
}


/*
���÷ְ�����
*/
void RadioPackageLengthSettingSet(eRadioPackageLengthSetting ePackageLength,stRadioCommandParameter *parameter,stRadioStatus *status)
{
	parameter->chan.bit.PackageLengthSetting=ePackageLength;
	RadioParameterSend(parameter,status);//���÷ְ�����
}

/*
	һЩ��ģʽ�Ĳ���
*/
/*
	�л�������ģʽ��ģʽ 3�� :�л�������ģʽ��ʼ���ö�Ӧ�Ĳ�������
baud:����ԭ�еĲ�����
*/
void RadioConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud )
{
	*baud=parameter->sped.bit.SerialRate;
	RaModeSwitch(eRaConfigurationMode,status);
	//�������ô��ڵĲ�����
	RadioUartBoundMatch(eRadioSerialRate_9600);
}
/*
	�˳�����ģʽ
	�жϲ�������û�иı�
baud: �������ݵĲ�����
*/
void RadioExitConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud)
{
	if(parameter->sped.bit.SerialRate!=eRadioSerialRate_9600)
	{
		//��ԭԭ���Ĳ�����
		RadioUartBoundMatch(*baud);
	}
	RaModeSwitch(eRaSubcontractMode,status);//��ԭ���ְ�ģʽ
}
/*
	��������
	ƴ�����õĲ���
*/
void RadioParameterSet(
	stRadioCommandParameter *parameter,//���ص�̨�Ĳ���
	u8 addH,	//�ߵ�ַ
	u8 addL,	//�͵�ַ
	eRadioParameterSave	ParameterSave,	//����ͷ
	eRadioCmdSerialRate SerialRate, 	//���ڲ�����
	eRadioCmdAirRate 	AirRate,		//���ٶ�
	eRadioSentToSent 	SentToSent		//���㷢��ʹ��λ

)
{
	parameter->addH=addH;
	parameter->addL=addL;
	parameter->head=ParameterSave;
	parameter->sped.bit.SerialRate=SerialRate;
	parameter->sped.bit.AirRate=AirRate;
	parameter->option.bit.SentToSent=SentToSent;
}
/*
���͵�̨���������õ�̨
*/

void RadioParameterSend(stRadioCommandParameter *parameter,	stRadioStatus *status)
{
	u8 baud=0;
	u8 len=0;
	len=sizeof(stRadioCommandParameter);
	RadioConfigurationMode(parameter,status,&baud );//�л�������ģʽ
	RadioSendByteSLen((char*)parameter ,len);
	
//	RadioSendByteSizeof((char*)parameter);					//����������Ϣ
	RadioExitConfigurationMode(parameter,status,&baud);//�˳�����ģʽ
}
/*
��ʼ����̨
*/
void RadioInit(stRadioStatus *status,stRadioCommandParameter *parameter)
{
	//��ʼ��IO
	RadioGpioInit( );
	//���ó�ʼ������
	RadioParameterInit(RADIO_BROADCAST_MODE_ADD_H,RADIO_BROADCAST_MODE_ADD_L,parameter);
	//������Ҫ�Ĳ���
	RadioParameterSet(parameter,RADIO_BROADCAST_MODE_ADD_H,RADIO_BROADCAST_MODE_ADD_L,eRadioParameterSaveY,eRadioSerialRate_9600,eRadioAirRate_12k,eRadioSentToSent_FixedPoint);
	RadioParameterSend(parameter,status);//�������õĲ���

}

/*
	��̨���ݷ��ͣ�ͨ����̨�������ݷ��͸�����������豸
	�����͵�������GGA��RTCM����
*/

void RadioDataSend(stRadioCommandParameter *parameter,char *data)
{	
	char *p;
	uint16_t data_len=0;
	data_len=strlen(data);
#if 0
	p=malloc(data_len+RADIO_DATA_HADE_LEN);
	memset(p,0,data_len+RADIO_DATA_HADE_LEN);
	*p=parameter->addH;
	*(p+1)=parameter->addL;
	*(p+2)=parameter->chan.bit.CommunicationChannel;
	memcpy(p+RADIO_DATA_HADE_LEN,data,data_len);
	RadioSendByteSLen(p ,data_len+RADIO_DATA_HADE_LEN );
	free(p);
#else
	printf("data_len=%d\r\n",data_len);
	p=malloc(data_len);
	
	memset(p,0,data_len);
	memcpy(p,data,data_len);
	RadioSendByteSLen(p ,data_len );
//	RadioSendByteSizeof(data);//��������
#endif

}

/*

	��̨��������
	�������̣�
	1��RTK�ϴ�GGA���ݵ��������ȴ�rtk������ɣ���վ�ﵽ�̶��⣬ȷ����վ��λ�ã��������������һ����׼�ľ�γ�ȣ�����ȷ����վ��λ�úͻ�վ���źŷ��䷶Χ
	2���ȴ�����������һ��������Ϣ����ʾ�������Ѿ�ȷ����վλ�ã��ȴ������������ŵ�������ŵ������õ�̨�����úú��л�������ģʽ����ʼ�����Ե�
���䣨GGA��RTCM���ݣ�
	
*/
void RadioMain(void)
{
	stRadioStatus status;//״̬�ļ�¼
	stRadioCommandParameter parameter;//������¼
}

