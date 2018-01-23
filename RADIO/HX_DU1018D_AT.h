#ifndef __HX_DU1018D_AT_H_
#define __HX_DU1018D_AT_H_

//�����ͨ������
#define	HX_COMMAND_END "\r\n"
#define	HX_COMMAND_MAX_LEN  50


//��һ���������AT���� 

#define AT_W "AT&W"//������������  �������AT&W+�س����� ���óɹ����ء�OK�� 
#define  FREQUENCY_TABLE_MAX_NUM 7
extern char pDU1018DFrequencyTable[FREQUENCY_TABLE_MAX_NUM][sizeof("ATP0=00 450.125 450.125\r\n")];//400M��Ƶ  ����Ƶ�ʱ����� 
#define AT_V "AT&V"//��ǰ��̨����������ʾ �������AT&V+�س�����  ���óɹ����ء�OK�� 
//�ڶ��������ͨAT�������� 
/*
Ĭ������
ATS102=1-115200;
ATS103=2-9600
ATS108=NO
ATS131=NO
ATS132=NO
*/
#define AT_SerialBaud 		"ATS102="	//���ڲ������޸� �������ATS102=value+�س����� 1-115200��2-57600��3-38400��5-19200��7-9600�� 
#define AT_AeriallBaud 		"ATS103="	//���в������޸� �������ATS103=value+�س����� խ��ģʽ��0-4800;1-8000;2-9600;3-16000;4-19200;
#define AT_TransmitPower 	"ATS108="	//���书���޸� �������ATS108=value+�س����� H:�߹��ʣ�L���͹��ʣ�
#define AT_TXChannelSettings "ATS131="	//��ǰ��̨��������ͨ������  valueȡֵ��Χ 0-63 
#define AT_RXChannelSettings "ATS132="	//��ǰ��̨��������ͨ������  valueȡֵ��Χ 0-63 
#define AT_Communication	 "ATS186="	//ͨ��Э��ѡ�� 1-TRIMTALK; 2-TRIMMK3; 4-TT450S; 5-TRANSEOT 

//���ڲ������޸�
typedef enum  eUsartBaud102
{
	eSerialBaud_115200 = '1',
	eSerialBaud_57600,
	eSerialBaud_38400,
	eSerialBaud_28800,
	eSerialBaud_19200,
	eSerialBaud_14400,
	eSerialBaud_9600,

}eUsartBaud102;
//���в������޸�
typedef enum  eAeriallBaud103
{
	eAeriallBaud_4800 = '0',
	eAeriallBaud_8000,
	eAeriallBaud_9600,
	eAeriallBaud_16000,
	eAeriallBaud_19200,
}eAeriallBaud103;

//���书���޸� ��������
typedef enum  eTransmitPower108
{
	eHighPower = 'H',
	eLowPower = 'L',
}eTransmitPower108;


//ͨ��Э��ѡ��
typedef enum  eCommunication186
{
	eTRIMTALK='1',
	eTRIMMK3,
	eTT450S='4',
	eTRANSEOT,
}eCommunication186;
//������û�в���typedef
typedef enum  eAtDu1018dCommandParameter
{
	eAtDu1018dCommandParameterY = 0,//�в���
	eAtDu1018dCommandParameterN ,
}eAtDu1018dCommandParameter;
/*
����ƴ�ӣ�
command:ƴ�ӵ�����
parameter������Ĳ���
parameterY_N����û�в����ı��
return_command�������
*/
extern void AtDu1018dCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,char *return_command);
#endif 

