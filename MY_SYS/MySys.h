

/*
本文件的定义：
1：全局的状态的记录和标记
2：全局函数的定义
*/

#ifndef __MY_SYS_H_
#define __MY_SYS_H_
#include "sys.h"

#define sw16(x) ((short)((((short)(x)&(short)0x00ff)<<8)|(((short)(x)&(short)0xff00)>>8)))
#define sw32(x)	((int)((int)(sw16((x)&(int)0xffff)<<16))|((int)(sw16(((x)&(int)0xffff0000)>>16))))

#define sw16_uin16(x) ((uint16_t)((((uint16_t)(x)&(uint16_t)0x00ff)<<8)|(((uint16_t)(x)&(uint16_t)0xff00)>>8)))
#define sw32_uint32(x)	((uint32_t)((uint32_t)(sw16_uin16((x)&(uint32_t)0xffff)<<16))|((uint32_t)(sw16_uin16(((x)&(uint32_t)0xffff0000)>>16))))
/*
全局变量定义:
*/
extern  char gSysGgaDataBuffer[100];//GGA数据刷新缓存池
//int 转化成字符
extern   void  uint32_to_character(char * des ,uint32_t src);
//字符 转化成 int  char_len是des字符串的长度
extern   uint32_t  character_to_uint32(char * des ,uint8_t char_len);

#define  RADIO_E71 0
#define  RADIO_DU1018D 1
#endif


