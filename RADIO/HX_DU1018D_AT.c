/*
��̨�����ԣ�
1��	+++ ����ģʽ�л�����������ԣ������ַ�����+++��(ע�ⷢ�ͷ���������+++֮ǰ1S�ڲ�
	��������,����+++֮�� 1S���ڲ��������ݣ��ҷ���+++���治�ܴ��س����з�)������̨�����������ģʽ����
	�ԡ�NO CARRIER OK����

*/
#include "HX_DU1018D_AT.h"
#include "string.h"
/*
��̨��ATָ�����ĵ�
*/

//400M��Ƶ  ����Ƶ�ʱ����� 
char pDU1018DFrequencyTable[FREQUENCY_TABLE_MAX_NUM][sizeof("ATP0=00 450.125 450.125\r\n")]=
{
	{"ATP0=00 450.125 450.125\r\n"},
	{"ATP0=01 451.125 451.125\r\n"},
	{"ATP0=02 452.125 452.125\r\n"},
	{"ATP0=03 453.125 453.125\r\n"},
	{"ATP0=04 454.125 454.125\r\n"},
	{"ATP0=05 455.125 455.125\r\n"},
	{"ATP0=06 456.125 456.125\r\n"},
};

/*
����ƴ�ӣ�
command:ƴ�ӵ�����
parameter������Ĳ���
parameterY_N����û�в����ı��
return_command�������
*/
void AtDu1018dCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,char *return_command)
{
	char* p_return =return_command;
	switch(parameterY_N)
	{
		case   eAtDu1018dCommandParameterY://�в��� 
			
			p_return=strcat (p_return,command);
			p_return=strncat (p_return,parameter,1);
			break;
		case   eAtDu1018dCommandParameterN://û�в���
			p_return=strcat (p_return,command);
			break;
		 default: break;
	}
	p_return=strcat (p_return,HX_COMMAND_END);
	#if DEBUG_HX_DU1018D
		printf("DU1018D command : %s",p_return);
	#endif
}







