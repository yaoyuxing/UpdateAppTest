#ifndef __RTK_H__
#define __RTK_H__
#include "sys.h"
#include "uart4.h"
#include "rtk.h"
#include "me3630.h"
//strcmp()
//   <0               ��һ���ַ���С�ڵڶ����ַ���
//     0               �����ַ������    ��
//    >0               ��һ���ַ������ڵڶ����ַ���
#define ToString(x) #x  //��X�����е����ݼ���""
#define RTK_COMMAND_COM  "com1"
#define RTK_DATA_COM  "com2"

#define RTK_DATA_COM_BAUD_INIT  "9600"//Ĭ�ϲ�����
#define RTK_DATA_COM_BAUD_SET  	"57600"//��Ҫ���õĲ�����

#define MAX_RTK_COMMAND_BUF  200

/*
���ڵ���ض���
*/
//rtcm���ݶ˿�

#define RtkSendRtcmUartInit(bound)  uart4_init(bound)
#define RtkSendRtcmToRtk(data,len)	UART4_SendNumBytes(data,len)//��������RCM���ݵ�RTK
#define RtkSendRtcmUartByte(data) 	UART4_SendByte(data)

//����˿�
#define RtkSendCommandUartInit(bound)  UART3_Bound(bound)
#define RtkSendCommandUartChar(data)  rtkSendByte(data)
#define RtkSendCommandUartChars(data,len) UART3_SendDataBytes(data,len)

/*
���ڵ���ض��� end
*/
extern char gCommandBuf[MAX_RTK_COMMAND_BUF];

#define   POSITION(X)    (0x1<<(X))

/*
RTK״̬����
*/
//�ж�RTK�������Ƿ��ͳɹ�
enum  eRtkSendCommand
{
	eSendCommandY=0,
	eSendCommandN,
};
//�ж�RTK�������Ƿ��ͳɹ�
enum  eRtkGgaStatus
{
	eGpggaStatus1=0,
	eGpggaStatus4,  //GGA��״̬Ϊ4
	eGpggaStatus7,
	eGpggaStatusN,
};
//�ж�BESTPOS�������Ƿ��ȡ�ɹ�
enum  eRtkBestposStatus
{
	eBestposStatusY=0,  
	eBestposStatusN,
};
//�ж��Ƿ�����˻�վģʽ
enum  eRtkBaseStation
{
	eBaseStationY=0,  
	eBaseStationN,
};
//gga�Ļ����� 
enum  eRtkGetGga
{
	eGetGgaY=0,  
	eGetGgaN,
};
typedef  struct 
{
	uint32_t	send_command:1;//�жϷ���������û�����óɹ�eRtkSendCommand
	uint32_t	gpgga_status:2;//��ʾ״̬��û�е���״̬4
	uint32_t	bestpos_status:1;//eRtkBestposStatus
	uint32_t	base_station:1; //eRtkBaseStation
	uint32_t	get_gga:1;//gga�Ļ�����  o����óɹ�  1�����ʧ��
	uint32_t	:32-6;

}stRtkStatusBit;
typedef union{
	uint32_t   data;
	stRtkStatusBit bit;
}stRtkStatus;
/*
RTK���� γ�� �߶�
γ�ȣ�latitude
���ȣ�Accuracy
�߶ȣ�heigh
*/
#define MAX_BESTPOS_DATA_LEN 20
typedef  struct 
{
	char	latitude[MAX_BESTPOS_DATA_LEN];//γ�ȣ�
	char	accuracy[MAX_BESTPOS_DATA_LEN];//���ȣ�
	char	heigh[MAX_BESTPOS_DATA_LEN];//�߶ȣ�
}stRtkBestposData;
/*
RTK״̬����end
*/
/*
�ⲿ�����ӿ�
*/
extern void rtk_SendBytes(char *command);
/*
����ָ�����ȵ����ݵ�RTK
*/
extern void rtk_SendNumByte(char *data ,uint16_t len);
/*
����RTK GGA��״̬����
[COM2]$GPGGA,100625.00,3030.5032,N,10407.1052,E,7,08,2.0,502.33,M,-43.20,M,,*4F
*/
extern void RtkGpggaStatus(stRtkStatus *RtkStatus); 
/*
�ⲿ�����ӿ� end
*/
extern void RtkBestposAnalysis(stRtkBestposData *BestposData,stRtkStatus *RtkStatus);
/*
ƴ��LOG����
GGA
bestpos
log com2 gpgga ontime 1
*/
extern void LogCommand(char *command,char *comx,char *delaytime ,char* return_command);
/*
���ý����׼վģʽ������ƴ��
fix position γ�� ���� �߶ȣ������γ�ȸ߶���bestpos�ģ�
γ�ȣ�latitude
���ȣ�Accuracy
�߶ȣ�height
*/
extern void FixPositionCommand(char *latitude,char *accuracy ,char* height,char* return_command);
/*
���ý����׼վģʽ������ƴ��
fix none
*/
extern void FixNoneCommand(char* return_command);
/*
com com2 115200
����COM�Ĳ�����
*/
extern void RtkComBaudCommand(char * comx,char *baud,char* return_command);
/*
����com�ڵ��շ�Э��
interfacemode com2 rtcmv3 novatel on
INTERFACEMODE [port] rxtype txtype [responses]
*/
extern void RtkInterfaceCommand(char * comx,char *rxtype,char *txtype,char* return_command);

/*
RTK��λ����������
freset
*/
extern void RtkFreset(char* return_command);
/*
�رն�ӦCOM�ڵĶ�Ӧ�������
unlog com1 gpgga 
*/
extern void UnLogCommand(char *comx,char *command,char* return_command);
/*
�ر�����COM�ڵ��������
unlogall
*/
extern void UnLogAllCommand(char* return_command);
/*
�����������COM���������
*/
extern void RtkSendUnlogallcom(char *com,stRtkStatus *RtkStatus);
/*
����������ж�������û�����óɹ�
RtkStatus��RTK�ķ���״̬
*/
extern void RtkSendCommandStatus(stRtkStatus *RtkStatus);
/*
����RTK��������
command�����͵�����
RtkStatus��RTK�ķ���״̬
*/
extern void RtkSendCommand(char *command,stRtkStatus *RtkStatus);
/*
����RTK GGA��״̬����
[COM2]$GPGGA,100625.00,3030.5032,N,10407.1052,E,7,08,2.0,502.33,M,-43.20,M,,*4F
*/
extern void RtkGpggaStatus(stRtkStatus *RtkStatus);
/*
����Bestpos���ݵ�
*/
extern void RtkBestposAnalysis(stRtkBestposData *BestposData,stRtkStatus *RtkStatus);
/*
����GGA�����ȡ״̬
log com2 gpgga ontime 1
*/
extern void RtkSendGpgga(stRtkStatus *RtkStatus,char *time);
/*
����GGA�����ȡ״̬:ָ��һ���˿�
comx:rtk�˿ں�
time������Ƶ��
log com2 gpgga ontime 1
*/
extern void RtkSendGpggaComx(char * comx,char *time,stRtkStatus *RtkStatus);
/*
RTK��ʼ����
��ʼ״̬���ݳ�ʼ��
*/
extern void RtkInit(stRtkStatus *RtkStatus);
/*
RTCM���ݷ���,�ж�gga��״̬��û�е���4
*/
extern void RtkSendRtcmDataStatus(stRtkStatus *RtkStatus,char *rtcm_data);
/*
�������ܣ�
1������RTK���LOG GGA������
2������GGA�����ݣ��ж�GGA��״̬
3����GGA���ݷ���������
ͬʱ����һ�������ж���û���յ�rtcm����
������յ������ݷ��͸�RTK������me3630RxAnalysis��
*/
extern void RtkGetGga(stRtkStatus *RtkStatus,stMe3630Status *Me3630Status);
/*
RTK:��У׼
	У׼���̣��û�վ�Լ���GGA����
*/

extern void RtkSelfCalibration(stRtkStatus *RtkStatus);
/*
���ý����׼վģʽ
fix position γ�� ���� �߶�
*/

extern void RtkSendPosition(stRtkBestposData *BestposData,stRtkStatus *RtkStatus);
/*
��У׼��վģʽ����
state = on off
maxtime:�ȴ�ʱ��
maxhstd:ˮƽ���
maxvstd���߶����
*/

extern void RtkCommandPosave(char *state,char *maxtime,char *maxhstd ,char *maxvstd ,char* return_command);

/*
RTK��״̬��ǳ�ʼ��
*/

extern void RtkStatusInit(stRtkStatus *RtkStatus);

extern void rtk_test(void);



extern stRtkStatus	gRtkStatus;
#endif
