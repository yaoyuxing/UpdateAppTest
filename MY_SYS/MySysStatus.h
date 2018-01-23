#ifndef __MY_SYS_STATUS_H_
#define __MY_SYS_STATUS_H_
/*
系统状态：（随时都在变的）
1：RTK的状态
2：电量
3：工作的状态（初始化中，正常工作，有错误）
4：系统错误状态编号（对应模块编号，模块内部的错误编号）
5：

*/

/*
系统信息：（设置后不改变或者需要存入存储的）
1：设备ID
2：通道号
3：工作模式（基站，手持杖）
4：
*/

#include "sys.h"
/***********************************************************************************************
						系统的工作状态
***********************************************************************************************/
typedef enum	eMySys_WorkMode
{
	eMySys_HandheldMode =0,  //手持杖模式
    eMySys_BaseStationMode,		//基站模式
	eMySys_ModeSwitch,			//切换中
}eMySys_WorkMode;

//系统的状态标记
typedef  struct 
{
	uint32_t WorkMode:2; //工作模式 eMySys_WorkMode   在蓝牙中更新
	uint32_t CommunicationChannel :5;//通信信道
	uint32_t		:32-7;
}stMySysStatusBit;
typedef union{
	uint32_t			data;
	stMySysStatusBit	bit;
}stMySysStatus;
extern stMySysStatus  gMySysStatus; //标记系统的状态的全局变量

/***********************************************************************************************
						定义系统的信息（掉电存储的参数）
***********************************************************************************************/
//系统信息数据类型结构体
#pragma pack(1)
//一些需要被记录的位

typedef struct{
	u8	RadioChannel;	//电台通道
	u8 WorkMode; 		//工作模式 eMySys_WorkMode   在蓝牙中更新
	
}stMySysInformation;
extern stMySysInformation  gMySysInformation;	//系统信息的全局变量
#define pMYSYS_INFORMATION_ADD    

#pragma pack()








#endif
