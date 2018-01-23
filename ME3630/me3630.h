#ifndef __ME3630_H__
#define __ME3630_H__
#include "sys.h"
/*
���������б�
*/
#define AT				"AT\r"
#define	CALL_OPEN		"AT+ZIPCALL=1\r" 							//��Э��
//#define	TCP_OPEN		"AT+ZIPOPEN=1,0,120.76.224.208,60000\r"  	//����tcpЭ��
#define	TCP_OPEN		"AT+ZIPOPEN=1,0,47.92.131.173,8096\r"  	//����tcpЭ��
#define	CALL_CLOSE		"AT+ZIPCALL=0\r"   							//�ر�Э��
#define	TCP_CLOSE		"AT+ZIPCLOSE=1\r"  							//�ر�TCP
#define	CSQ				"AT+CSQ\r"  								//�ź�ǿ�ȶ�ȡ

#define	SEND_ORIGINAL_DATA	"AT+ZIPSENDRAW=1,"						//����ԭʼ����
#define	SEND_DATA		"AT+ZIPSEND=1,"								//��������

#define COMMAND_END		"\r\0"										//������
/*
���������б�  end
*/
#define MAX_SENDCOMMAND_NUM  20			//�ظ���������Ĵ���
#define	MAX_SENDDATA_NUM     200		//����͵��ֽ���
#define	SENDTIME_MS     	 200		//���͵ļ��ʱ��
#define	MAX_EXTRACTION_DATA_LEN     	 800		//������������ݳ��� char //600
/*
����״̬�б�
*/
//�ַ�״̬
#define	COMMAND_OK		"OK"					//�������ȷ
#define	COMMAND_ERR		"ERROR"					//����ʹ���
/*
	AT+CSQ �ź�ǿ�Ȳ�ѯ
	���������ݶ�Ӧ��
	��ָ�����ڲ�ѯ�����ź�ǿ�ȣ�rssi�����ŵ�������(ber) 
*/



//�������ͳɹ�
enum  eMe3630CommandStatus
{
	eSendCommandOk=0,
	eSendCommandErr,
	eSendCommandIng,
};
//������ݷ���
enum  eMe3630DataStatus
{
	eSendDataOk=0,//���ͳɹ�
	eSendDataErr,//����ʧ��
	eSendDataIng,//���ڷ���
};
//�������ͳ�ʱ
enum  eMe3630CTimeoutStatus
{
	eTimeoutY=0,
	eTimeoutN,
};
//TCP	0���ӳɹ� 1û�����ӳɹ�
enum  eMe3630TcpLinkStatus
{
	eLinkY=0,
	eLinkN,
	eLinkIng,
};
//0��ʾģ�鿪��׼������   1��ʾ��û�о���
enum  eMe3630OnStatus
{
	eOn=0,
	eOff,
};


//����Ƿ񲻶�ѭ������ 1����ѭ�� 0��ʾ��һֱѭ��
enum  eMe3630SendCtrCycleAlways
{
	eAlwaysN=0,
	eAlwaysY,
};
/*
�������������ʱ�Ŀ���
*/
typedef  struct 
{
	uint32_t	cycle_nunm:8;//�������ѭ������
	uint32_t	time_len:16;//ÿ���������ʱʱ�䵥λms
	uint32_t	cycle_always:1;//����Ƿ񲻶�ѭ������ 1����ѭ�� 0��ʾ��һֱѭ��
	uint32_t	:7;
}stMe3630CtrBit;
typedef union{
	uint32_t   data;
	stMe3630CtrBit bit;
}stMe3630Ctr;
/*
�������������ʱ�Ŀ���end
*/

/*
��¼4Gģ�鷢�͵����״̬
*/
//����Ƿ񲻶�ѭ������ 1����ѭ�� 0��ʾ��һֱѭ��
enum  eMe3630SendRaw
{
	eSendRawY=0,
	eSendRawN,
};

typedef  struct 
{
	uint32_t send:1;//�������ͳɹ� 0Y  1N  �յ�OK
	uint32_t send_raw:1;//��Ƿ���ԭʼ��������ͳɹ�  0Y  1N  �յ�NO
	uint32_t :30;
}stMe3630SendStatusBit;
typedef union{
	uint32_t data;
	stMe3630SendStatusBit bit;
}stMe3630SendStatus;

/*
��¼4Gģ�鷢�͵����״̬
*/

/*
��¼4Gģ������״̬
*/
//����Ƿ񲻶�ѭ������ 1����ѭ�� 0��ʾ��һֱѭ��
enum  eMe3630Receive
{
	eReceiveFullY=0,
	eReceiveFullN,
};
//��ȡRTCM������û�гɹ�  0�ɹ�  1ʧ��
enum  eMe3630GetRtcm
{
	eGetRtcmY=0,
	eGetRtcmN,
};
#define MAXTCPPAKNUM  1024
typedef  struct 
{

	uint32_t	call_timeout:1;  //��Э��0��ʱ 1û�г�ʱ eMe3630CTimeoutStatus
	uint32_t	tcp_timeout:1;  //tcp����Э��0��ʱ 1û�г�ʱ eMe3630CTimeoutStatus
	uint32_t	tcp_link:2;  //0���ӳɹ� 1û�����ӳɹ� 2�������� eMe3630TcpLinkStatus
	uint32_t	send_commd:2; //0�ɹ�  1ʧ��   2���ڷ���  eMe3630CommandStatus
	uint32_t	on:1; //0��ʾģ�鿪��׼������   1��ʾ��û�о���
	uint32_t	receive:1;// 0���������� 1 ��ʾ����û�н�����    
	uint32_t	read:1;// 0 ��ʾ������û�ж���    1��ʾ�������յ������ֵ
	uint32_t	send_data:2;//���ݷ���״̬��¼  0�ɹ�  1ʧ��   2���ڷ���  eMe3630DataStatus
	uint32_t	get_rtcm:1;//��ȡRTCM������û�гɹ�  0�ɹ�  1ʧ��
	uint32_t	:(32-12);

}stMe3630StatusBit;

typedef union{
	uint32_t   data;
	stMe3630StatusBit bit;
}stMe3630Status;
extern stMe3630Status  Me3630Status;
/*
��¼4Gģ������״̬ end
*/

/*
����״̬�б�end
**************************************************************/
/*
api��ʶ��
����	�ӿ�		ע��
1	����������Ӧ	���ڽ��ܵ����ݺ󣬷��������ݷ��ͷ���ȷ���յ���Ϣ��
11	GGA����			���������������GGA����:����gga�ַ�����ǧѰ����
12	��վ��Ϣ�ϴ�	�ϴ���վ��״̬��Ϣ���ϴ���վ�������
*/
typedef enum  eMe3630ApiLogo
{
	eApiLogoRequestResponse=1,//���ڽ��ܵ����ݺ󣬷��������ݷ��ͷ���ȷ���յ���Ϣ��
	eApiLogoGGA=5,//���������������GGA����:����gga�ַ�����ǧѰ����
	eApiLogoSysStatus=12,//�ϴ���վ��״̬��Ϣ���ϴ���վ�������
}eMe3630ApiLogo;
typedef enum  eMe3630DataOutWay//�������л���ʽ
{
	eDataOutWayMsgpack=1,
	eDataOutWayJson=2,
}eMe3630DataOutWay;
/*GGA����������״̬*/
typedef enum  eMe3630ReGgaStatus//��������������ݺ�õ���GGA��״̬
{
	ReGgaStatus_Y=1014,//��ȷ
	ReGgaStatusL_N=0,//���ش���
	ReGgaStatusQX_N=1002,//ǧѰ�������Ĵ������
}eMe3630ReGgaStatus;

/*
GGA�����ݽṹ����
*/

#pragma pack(1) 
typedef  struct{
	char data[400];
	int length;
	int ret;
	
}stMe3630reGgaData;
#pragma pack()
/*
���巢�͵������ݽṹ
*/

#pragma pack(1) 
typedef  struct 
{
	uint32_t	 ApiLogo;			//api��ʶ�룺���ɷ���������API�ӿ��б�
	uint32_t	RequestID;			//����ID������վ����ͷ��͸�������������ID�ţ�ʹ�����ID������Ϊ�ұ������ݵ�����ı�ʾ��
	uint32_t 	DataLen;			//���ݳ���(����������ͷ)
	uint32_t	DataOutWay;			//�������л���ʽ 1 msgpack ���л� 2Json
	uint32_t 	PackageInsideNum;	//�����ڷְ��ڵ����У���0��ʼ
	uint32_t	PackageNum;			//���ݵķְ���������СΪ1
}stMe3630SendHead;
#pragma pack()
/*
���巢�͵������ݽṹend
*/
/*
me3630���ݷ���
data:��������
len�����͵����ݳ���
status�����ݷ��͵�״̬
*/
extern void me3630_SendData(char *data,uint32_t len,stMe3630Status *status);
/*
����ָ�����ȵ�����
*/
extern void me3630_SendDataBytes(char*data ,u16 len);
/*
�ⲿ�����ӿ�
*/
/*
��¼TCP�����
*/
extern void openTCP(stMe3630Status *status);
/*
���յ������ݣ������
*/
extern void  me3630RxUnpacking(stMe3630Status *Status);
/*
��������ȡ�������
\r\n+ZIPRECV: 1,120.76.224.208,60000,180,
�������ݳ���
*/
extern uint32_t  me3630RxDataExtraction(u8 *src,u8 **des);
/*
	����me_3630����������
*/
extern void me3630_SendBytes(char *command);
//�Է��͵�ѭ������
extern void me3630SendCtr(stMe3630Ctr *ctr,uint8_t cycle_nunm, uint16_t time_len ,uint8_t cycle_always);
/*
��ȡ4G�ź�ǿ��
data :�������ź�ǿ������
status:״̬����
�����������
AT+CSQ 
+CSQ: 17,99 
 
OK 

*/

extern void Me3630GetSignal(char *data,stMe3630Status *status );

//��ʼ�����Ժ���
extern void me3630TestDataInit(stMe3630SendHead *data);
//��ʼ״̬��ʾ
extern void Me3630StatusInit(stMe3630Status *Me3630Status);
/*
4Gģ����Ժ�ʽ
*/
extern void me3630_test(void);
#endif
