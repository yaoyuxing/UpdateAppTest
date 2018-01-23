#ifndef __MY_SYS_STATUS_H_
#define __MY_SYS_STATUS_H_
/*
ϵͳ״̬������ʱ���ڱ�ģ�
1��RTK��״̬
2������
3��������״̬����ʼ���У������������д���
4��ϵͳ����״̬��ţ���Ӧģ���ţ�ģ���ڲ��Ĵ����ţ�
5��

*/

/*
ϵͳ��Ϣ�������ú󲻸ı������Ҫ����洢�ģ�
1���豸ID
2��ͨ����
3������ģʽ����վ���ֳ��ȣ�
4��
*/

#include "sys.h"
/***********************************************************************************************
						ϵͳ�Ĺ���״̬
***********************************************************************************************/
typedef enum	eMySys_WorkMode
{
	eMySys_HandheldMode =0,  //�ֳ���ģʽ
    eMySys_BaseStationMode,		//��վģʽ
	eMySys_ModeSwitch,			//�л���
}eMySys_WorkMode;

//ϵͳ��״̬���
typedef  struct 
{
	uint32_t WorkMode:2; //����ģʽ eMySys_WorkMode   �������и���
	uint32_t CommunicationChannel :5;//ͨ���ŵ�
	uint32_t		:32-7;
}stMySysStatusBit;
typedef union{
	uint32_t			data;
	stMySysStatusBit	bit;
}stMySysStatus;
extern stMySysStatus  gMySysStatus; //���ϵͳ��״̬��ȫ�ֱ���

/***********************************************************************************************
						����ϵͳ����Ϣ������洢�Ĳ�����
***********************************************************************************************/
//ϵͳ��Ϣ�������ͽṹ��
#pragma pack(1)
//һЩ��Ҫ����¼��λ

typedef struct{
	u8	RadioChannel;	//��̨ͨ��
	u8 WorkMode; 		//����ģʽ eMySys_WorkMode   �������и���
	
}stMySysInformation;
extern stMySysInformation  gMySysInformation;	//ϵͳ��Ϣ��ȫ�ֱ���
#define pMYSYS_INFORMATION_ADD    

#pragma pack()








#endif
