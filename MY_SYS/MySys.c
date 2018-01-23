


#include "MySys.h"
#include "MySysStatus.h"
#include <stdlib.h>
#include <math.h>
/*
全局变量定义
*/
char gSysGgaDataBuffer[100];//GGA数据刷新缓存池
stMySysStatus  gMySysStatus; //标记系统的状态
stMySysInformation  gMySysInformation;	//系统信息的全局变量


/*
将src转换成字符放在des中
*/
//int 转化成字符
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

//字符 转化成 int  char_len是des字符串的长度
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
系统信息刷新
*/
void MySysInformation(stMySysInformation  MySysInformation)
{



}

