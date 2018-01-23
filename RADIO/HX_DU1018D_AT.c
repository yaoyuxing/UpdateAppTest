/*
电台的特性：
1：	+++ 电塔模式切换的命令的特性：发送字符串“+++”(注意发送方法：发送+++之前1S内不
	能有数据,发送+++之后 1S秒内不能有数据，且发送+++后面不能带回车换行符)，即电台进入参数配置模式（回
	显“NO CARRIER OK”）

*/
#include "HX_DU1018D_AT.h"
#include "string.h"
/*
电台的AT指令，相关文档
*/

//400M定频  工作频率表配置 
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
命令拼接：
command:拼接的命令
parameter：命令的参数
parameterY_N：有没有参数的标记
return_command：命令返回
*/
void AtDu1018dCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,char *return_command)
{
	char* p_return =return_command;
	switch(parameterY_N)
	{
		case   eAtDu1018dCommandParameterY://有参数 
			
			p_return=strcat (p_return,command);
			p_return=strncat (p_return,parameter,1);
			break;
		case   eAtDu1018dCommandParameterN://没有参数
			p_return=strcat (p_return,command);
			break;
		 default: break;
	}
	p_return=strcat (p_return,HX_COMMAND_END);
	#if DEBUG_HX_DU1018D
		printf("DU1018D command : %s",p_return);
	#endif
}







