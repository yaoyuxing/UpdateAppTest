


#include "MySys.h"
#include "MySysStatus.h"
#include <stdlib.h>
#include <math.h>
/*
ȫ�ֱ�������
*/
char gSysGgaDataBuffer[100];//GGA����ˢ�»����
stMySysStatus  gMySysStatus; //���ϵͳ��״̬
stMySysInformation  gMySysInformation;	//ϵͳ��Ϣ��ȫ�ֱ���


/*
��srcת�����ַ�����des��
*/
//int ת�����ַ�
 void  uint32_to_character(char * des ,uint32_t src)
{
	int buf=0;
	buf=src;
	des[3]=(buf%10)+'0';
	buf=buf/10;
	des[2]=(buf%10)+'0';
	buf=buf/10;
	des[1]=(buf%10)+'0';
	buf=buf/10;
	des[0]=(buf%10)+'0';
}

//�ַ� ת���� int  char_len��des�ַ����ĳ���
 uint32_t  character_to_uint32(char * des ,uint8_t char_len)
{
	
	uint32_t return_data=0;
	uint8_t num=0;
	for(num=1;num<=char_len;num++)
	{
		
		return_data+=(*(des++)-'0')*(pow(10,char_len-num));
	}
	return return_data;
}
/*
ϵͳ��Ϣˢ��
*/
void MySysInformation(stMySysInformation  MySysInformation)
{



}

