#ifndef __AT_BLUETOOTH_H__
#define __AT_BLUETOOTH_H__
#include "bluetooth.h"
//������Խ�����
/*
0a------���з��ţ�����������"\n"
0d------�س����ţ�����������"\r"
0D 0A 
*/
#define BL_SEND_COMMAND_ECHO_1	'\r'
#define BL_SEND_COMMAND_ECHO_2	'\n'

//������� equal
#define	BL_EQUAL	"="
#define	BL_COMMAND_END	"\0"
//ģʽ�л�����
#define	BL_MODE		"+++"			//ģʽ�л�ָ��
#define	BL_ADVON	"AT+ADVON"		//��ģ��㲥����
#define	BL_ADVOFF	"AT+ADVOFF"		//�ر�ģ��㲥����
#define	BL_DISCONN	"AT+DISCONN"	//�Ͽ������ӵ��豸
#define	BL_RESET	"AT+RESET"		//��λָ��
#define	BL_FACTORY	"AT+FACTORY"	//�ָ���������
//ֻ����ѯ������
#define	BL_VER		"AT+VER"		//��ѯ�汾��
#define	BL_MAC		"AT+MAC"		//��ѯģ�� MAC��ַ
#define	BL_SCAN		"AT+SCAN"		//ɨ�赱ǰ�ڹ㲥���豸
//��ѯ����������
#define	BL_NAME		"AT+NAME"		//��ѯ����ģ�� NAME
#define	BL_UART		"AT+UART"		//��ѯ���ô��ڲ�����
#define	BL_ROLE		"AT+ROLE"		//��ѯ����ģ���ɫ
#define	BL_TXPWR	"AT+TXPWR"		//��ѯ����ģ�鷢�书��
#define	BL_UUID		"AT+UUID"		//��ѯ�����豸 UUID
#define	BL_ADVDATA	"AT+ADVDATA"	//��ѯ���ù㲥����


#define	BL_ADVIN	"AT+ADVIN"		//��ѯ����ģ��㲥��϶
#define	BL_CONIN	"AT+CONIN"		//��ѯ����ģ�����Ӽ�϶

//����������
#define	BL_CONNECT	"AT+CONNECT"	//ָ�������豸
#define	BL_IBEACON	"AT+IBEACON"	//��ѯ���� IBEACON ����
#define	BL_RSSI		"AT+RSSI"		//��ȡ�������豸 RSSI
//���������ƴ��ʱƴ�ӳɲ�ѯģʽ��������ģʽ������
typedef enum  eBlCommandMode
{
	eBlCommandInquire=0, //��ѯģʽ
	eBlCommandSetUp,	//����ģʽ
}eBlCommandMode;

//��ѯ����ģ���ɫ
 enum  eBlCommandRole
{
	eBlCommandOrdinary=0, //���õ�ǰģ���ɫΪ��ͨģʽ
	eBlCommandMulti,//���õ�ǰģ���ɫΪ���ɫģʽ
};
#define   CMD_LEN_MAX         10
#define   COMP_LEN_MAX         20   //
#define   COMP_TYPE_MAX        2 //��ѯ����������������ҪУ��
#define   CMD_NUM_MAX              20
typedef  void (*SendCommadFunType)(char * ,char* ,eBlCommandMode );
typedef struct  cmd_ctrl_struct_base
{
	SendCommadFunType  pFunc;
	char     CmdStr[CMD_LEN_MAX];          //����
	char     CompareStr[2][COMP_LEN_MAX];  //У����ַ���
	unsigned short DelayTime;           //������ʱʱ��
}stCmdCtrlBaseType;

typedef struct  cmd_ctrl_struct
{
	 
 	stCmdCtrlBaseType stCmdCtrlBase[CMD_NUM_MAX] ;
	unsigned char     CmdNum;
}stCmdCtrlType;

/*�����ӿ�*/
/*��ѯ����ģ�� NAME
ִ��ָ �� AT+NAME=CdEbyte_MultiRole ���õ� ǰģ���� �֣����� �� 20 ���ַ� ���ɹ��� ���� PARA SET:
CdEbyte_MultiRole��
*/
extern void AtBlueNameCommand(char *name ,char* return_command,eBlCommandMode CommandMode);
/*��ѯ���ô��ڲ�����
ִ��ָ�� AT+UART=115200,8,1,0 ���õ�ǰģ�����֣��ɹ��󷵻� PARA SET: 115200,8,1,0��
��һ�����ݴ������ʣ�1200��2400��4800��9600��19200��38400��57600��115200��128000��230400��256000��
460800��921600��
�ڶ������ݴ�������λ��5,6,7,8��
���������ݴ���ֹͣλ��0,1��
���ĸ����ݴ���У��λ��0(none)��1(even)��2(odd))��
*/
extern void AtBlueUartCommand(char * baud,char* return_command,eBlCommandMode CommandMode);
/*��ѯ����ģ���ɫ
ִ��ָ�� AT+ROLE=0 ���õ�ǰģ���ɫΪ��ͨģʽ���ɹ��󷵻� PARA SET: Normal��
ִ��ָ�� AT+ROLE=1 ���õ�ǰģ���ɫΪ���ɫģʽ���ɹ��󷵻� PARA SET: MultiRole��
*/
extern void AtBlueRoleCommand(char * role,char* return_command,eBlCommandMode CommandMode);
/*��ѯ����ģ�鷢�书��
ִ��ָ�� AT+TXPWR=2dbm ���õ�ǰģ�鷢�书�ʣ��ɹ��󷵻� PARA SET: AT+TXPWR=2dbm��
���У����õĹ��ʷ�ΧΪ��2dbm��1dbm��0dbm��-3dbm��-6dbm��-9dbm��-12dbm��-15dbm��-18dbm��-21dbm��
*/
extern void AtBlueRoleTxpwr(char * txpwr,char* return_command,eBlCommandMode CommandMode);
/*��ѯ�����豸 UUID
ִ��ָ�� AT+UUID=0,FFF0 ���õ�ǰģ�����֣��ɹ��󷵻� PARA SET: AT+UUID=0,FFF0��
��һ���������õڼ��� UUID���䷶Χ 0 ~ 3��param =num+uuid ����"0,FFF0"*/

extern void AtBlueRoleUuid(char *param,char* return_command,eBlCommandMode CommandMode);
/*��ѯ���ù㲥���� ADVDATA
ִ�� AT+ADVDATA=0A00010203040506070809 ��ģ�齫���� PARA SET: 0x0A00010203040506070809��
��������Ĳ��� 0A ����Ҫ��������ݳ��ȣ�00010203040506070809 ������Ҫ��������ݡ��������ݰ�ʮ�����Ƹ�ʽ��
�룬�� 00 ���� 0x00��01 ���� 0x01��02 ���� 0x02�����ݳ��Ȳ����� 23 �ֽڡ�
*/
extern void AtBlueAdvdataUuid(char * advdata,char* return_command,eBlCommandMode CommandMode);



/*
����������������
uuid��������Ҫ�����ĺ���
void(*StringCommand)(char * ,char* ,eBlCommandMode ) //���ô������ĺ���  

data��Ҫ���͵���������
*/
extern void  BlSendSetUpCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ),char * data);
/*
����UUID��������
num_uuid:�ڼ���uuid
uuid:uuid��
*/
extern void  BlSendSetUpUuid(char * num_uuid,char *uuid);
/*
����������ѯ����
*/
extern void BlSendInquireCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ));
/*
һЩû�в������������һЩ��������
*/
extern void BlSendSwitchCommand(char *command);
/*
����һ������ȴ�״̬��ȷ
void(*StringCommand)(char * ,char* ,eBlCommandMode ):�����
data�����������
CompareEcho������Ƚϵ�����������
status���Ƚϵ�״̬
*/
extern void BlSendSetUpCommandEcho(void(*StringCommand)(char * ,char* ,eBlCommandMode ),eBlCommandMode eMode,char * data ,stBlStatus  *status);

 


#endif

