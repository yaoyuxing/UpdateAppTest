
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "oled.h"
#include "ChineseCharacter.h"
#include "MyGUI.h"


//图标隔开的竖线 
#define		X_VerticalLineInterval	2//到前图标的间隔
#define		X_VerticalLineWidth		2//线的宽度
/*上框位置定义*/
#define		X_START_HEAD  0//写起始位置的X坐标退位长度
#define		Y_START_HEAD  2//写起始位置的Y坐标下移动长度
//4G信号强度
#define		X_SignalStrengthIocnStart	X_START_HEAD //4G信号图标开始位置
#define		Y_SignalStrengthIocnStart	Y_START_HEAD //4G信号图标开始位置
#define		SignalStrengthIocnSize		12 //4G信号图标尺寸
#define		X_SignalStrengthIocnEnd		X_SignalStrengthIocnStart+SignalStrengthIocnSize //4G信号图标结束位置
#define		X_SignalStrengthCahrStart  	(X_SignalStrengthIocnEnd+4) //4G信号汉字开始位置“移动”
#define		X_SignalStrengthCahrEnd  		(X_SignalStrengthCahrStart+ChCh_4GSignalNUM*12+8) //4G信号汉字开始位置“移动”

//#define		SignalStrengthEnd   (X_SignalStrengthCahrEnd+X_START_HEAD) //4G信号结束显示的位置

//蓝牙显示位置定义
#define		BleWorkStatusStart	(X_SignalStrengthCahrEnd+8) //蓝牙状态显示开始位置
#define		X_BleWorkStatusStartIocnSize	(12) //蓝牙状态显示图标尺寸

#define		BleWorkStatusCahrStart  	(BleWorkStatusStart+X_BleWorkStatusStartIocnSize+2) //汉字开始位置
#define		BleWorkStatusEnd	(BleWorkStatusStart+X_BleWorkStatusStartIocnSize+12*(ChCh_BleConnectedNUM)+8) //蓝牙状态显示结束位置

//电池位置定义
#define		X_BatteryPowerStart (BleWorkStatusEnd+8)		//电池图标显示X坐标的开始位置
#define		Y_BatteryPowerStart	(Y_START_HEAD)		//电池图标显示y坐标的开始位置
#define		X_BatteryPowerIocnSize	(12) 				//电池图标显示图标尺寸
#define		X_BatteryPowerEnd 	(X_BatteryPowerStart+X_BatteryPowerIocnSize)		//电池图标显示X坐标的结束位置
#define		X_BatteryPower_VerticalLine_End 	(X_BatteryPowerEnd+X_VerticalLineWidth+X_VerticalLineInterval)		//电池图标竖线结束位置

/*
框位置定义
4条横y坐标定义
*/

#define OUTER_FRAME_Y_1 0
#define OUTER_FRAME_Y_2 15
#define OUTER_FRAME_Y_3 48
#define OUTER_FRAME_Y_4 63

#define OUTER_FRAME_X_1 0
#define OUTER_FRAME_X_2 127
/*
显示字符图标大小
*/
#define CHARACTER_ICON_SIZE  12

void  GUI_4GSignalStrengthChar(void);

static void GUI_VerticalLine(u8 x);
//电池电量  power共10个档位  (0到9)
void  GUI_BatteryPower(u8 power)
{
	u8 y=0,x=0;
	u8 x_start=0;
	x_start=X_BatteryPowerStart+1;
	OLED_ShowChineseString(X_BatteryPowerStart,Y_BatteryPowerStart,ChCh_BatteryIconNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_BatteryIcon);
	for(x=x_start;x<x_start+power;x++)
	{
		for(y=Y_BatteryPowerStart+3;y<Y_BatteryPowerStart+9;y++) 
		{
				OLED_DrawPoint(x,y,1);
		}	
	}
//	GUI_VerticalLine(X_BatteryPowerEnd+2);
	
}

/*
屏幕线框
*/
void  GUI_Wireframe(void)
{
	u8 num=0;
	for(num=0;num<128;num++)
	{
		if(num%3==0)
		{
			OLED_DrawPoint(num,OUTER_FRAME_Y_1,1);
			OLED_DrawPoint(num,OUTER_FRAME_Y_2,1);
	
			OLED_DrawPoint(num,OUTER_FRAME_Y_3,1);
			OLED_DrawPoint(num,OUTER_FRAME_Y_4,1);
		}
	}
	for(num=0;num<64;num++)
	{
		if(num%3==0)
		{
			OLED_DrawPoint(OUTER_FRAME_X_1,num,1);
			OLED_DrawPoint(OUTER_FRAME_X_2,num,1);
		}
	}
}

/*
信号强度：
一共4个信号强度：
*/

//强度的动态图标  Strength表示信号强度 共有四档信号强度
//Strength :表示信号强值(4)   x：图标起始位置(3) y:坐标的位置(2)

void  GUI_4GSignalStrengthDynamic(u8 Strength,u8 x,u8 y)
{
	u8 num=0;
	GUI_4GSignalStrengthChar();
	if(Strength==0)
	{
		OLED_ShowChineseString(x,y,1,CHARACTER_ICON_SIZE,1 ,(int)ChCh_4G_NoSignalIcon);

	}
	if(Strength>=1)
	{
		for(num=y+9;num<y+12;num++)
		{
			OLED_DrawPoint(x+1,num,1);//3
		}
	}
	if(Strength>=2)
	{
		for(num=y+6;num<y+12;num++)
		{
			OLED_DrawPoint(x+3,num,1);//5
		}
	}
	if(Strength>=3)
	{
		for(num=y+4;num<y+12;num++)
		{
			OLED_DrawPoint(x+5,num,1);//7
		}
	}
	if(Strength>=4)
	{
		for(num=y+2;num<y+12;num++)
		{
			OLED_DrawPoint(x+7,num,1);//9
		}
	}
	if(Strength>=5)
	{
		for(num=y;num<y+12;num++)
		{
			OLED_DrawPoint(x+9,num,1);//9
		}
	}
}
//画长度12一个像素点的竖线 x坐标的位置  //图标间隔竖线显示  12的字 +2上下横线+1的预留空间（上）=15
static void GUI_VerticalLine(u8 x)
{
	u8 num=0;
	for(num=0;num<15;num++) 
	{
		if(num%2==0)
		{
			OLED_DrawPoint(x,num,1);
			OLED_DrawPoint(x+1,num,1);
		}
	}
}
	
//4g信号强度的静态显示
void  GUI_4GSignalStrengthChar(void)
{
	GUI_VerticalLine(X_SignalStrengthCahrEnd);
	OLED_ShowChineseString(X_SignalStrengthCahrStart,Y_START_HEAD,ChCh_4GSignalNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_4G_Buffer);
}


//蓝牙的连接状态
void GUI_BleWorkStatus(eGUI_BleStatus  BleStatus)
{
	switch(BleStatus)
	{
		case eGUI_BleNoConnection: ////没有连接
			OLED_ShowChineseChar(BleWorkStatusStart,Y_START_HEAD,0,CHARACTER_ICON_SIZE,1 ,(int )ChCh_BleIconStatus);
			OLED_ShowChineseString(BleWorkStatusCahrStart,Y_START_HEAD,ChCh_BleNoConnectedNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_BleNoConnected);
			GUI_VerticalLine(BleWorkStatusEnd);
		break;
		case eGUI_BleConnected:////已经连接
			OLED_ShowChineseChar(BleWorkStatusStart,Y_START_HEAD,0,CHARACTER_ICON_SIZE,1 ,(int )ChCh_BleIconStatus[1]);
			OLED_ShowChineseString(BleWorkStatusCahrStart,Y_START_HEAD,ChCh_BleConnectedNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_BleConnected);
			GUI_VerticalLine(BleWorkStatusEnd);
		break;
		default: break;
	}
}



/*
下边框：相关功能定义
*/
#define		X_VerticalLineGap 14		//s竖线的前后间隙

#define		X_UnderBorderStart  0   				//下边框图标显示X坐标的开始位置
#define		Y_UnderBorderStart  (OUTER_FRAME_Y_3)	//下边框图标显示Y坐标的开始位置

//设备模式切换显示位置 
#define		X_WorkModeStart		14
#define		Y_WorkModeStart		(Y_UnderBorderStart+2)

#define		X_WorkModeVerticalLineStart		(X_WorkModeStart+X_VerticalLineGap+ChCh_ModeSwitchNUM*12)
#define		X_WorkModeVerticalLineEnd		(X_WorkModeVerticalLineStart+X_VerticalLineGap)
//设备工作状态显示位置

#define		X_EquipmentWorkStatusStart		X_WorkModeVerticalLineEnd
#define		Y_EquipmentWorkStatusStart		(Y_UnderBorderStart+2)

#define		X_EquipmentWorkStatusVerticalLineStart		(X_EquipmentWorkStatusStart+X_VerticalLineGap+3*12) //设备状态的显示开始位置
#define		X_EquipmentWorkStatusVerticalLineEnd		(X_WorkModeVerticalLineStart+X_VerticalLineGap)           //设备状态的显示结束位置

//下边框图标隔开的竖线
static void GUI_UnderBorderVerticalLine(u8 x)
{
	u8 y=0;
	for(y=OUTER_FRAME_Y_4-14;y<OUTER_FRAME_Y_4;y++) 
	{
		if(y%2==0)
		{
			OLED_DrawPoint(x,y,1);
			OLED_DrawPoint(x+1,y,1);
		}
	}
}
//设备模式切换
void GUI_WorkMode(eGUI_WorkMode WorkMode)
{
	GUI_UnderBorderVerticalLine(X_WorkModeVerticalLineStart);
	switch(WorkMode)
	{
		case eGUI_BaseStationMode:
//			OLED_ShowChineseString(X_WorkModeStart,Y_UnderBorderStart,ChCh_EquipmentWorkStatusIconNum,12,1 ,(int)ChCh_EquipmentWorkStatusIcon);
			OLED_ShowChineseString(X_WorkModeStart,Y_WorkModeStart,ChCh_BaseStationModeNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_BaseStationMode);
		break;
		case eGUI_HandheldMode:
			OLED_ShowChineseString(X_WorkModeStart,Y_WorkModeStart,ChCh_HandheldModeNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_HandheldMode);
		break;
		case eGUI_ModeSwitch:
			OLED_ShowChineseString(X_WorkModeStart,Y_WorkModeStart,ChCh_ModeSwitchNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_ModeSwitch);
		break;
		default: break;
	}
}

//设备工作状态
void GUI_EquipmentWorkStatus(eEquipmentWorkStatus EquipmentWorkStatus)
{
#if 0//有图标
	OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusIconNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusIcon);
	switch(EquipmentWorkStatus)
	{
		case eEquipmentWorkStatussY: //正常工作
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusYNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusY);
		break;
		
		case eEquipmentWorkStatussN: //初始化中
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusNNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusN);
		break;
		default: break;
	}
#endif
	
	switch(EquipmentWorkStatus)
	{
		case eEquipmentWorkStatussY: //正常工作
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusYNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusY);
		break;
		
		case eEquipmentWorkStatussN: //初始化中
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusNNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusN);
		break;
		default: break;
	}
}


/*
见面的数据状态刷新
*/
#include "me3630.h"
#include "bluetooth.h"
#include "MySysStatus.h"
void GUI_CtrDataRefresh(stGUICtrData  *GUICtrData)
{
	u8 NetworkSignalStrength=0;//存放4G的信号强度
	//4G信号强度读取
	/*
		0     –    <0.01%   
		1     –    0.01% --- 0.1%   
		2     –    0.1% --- 0.5%   
		3     –    0.5% --- 1.0%   
		4     –    1.0% --- 2.0%   
		5     –    2.0% --- 4.0%   
		6     –    4.0% --- 8.0%   
		7     –    多余 8.0% 
		99    无网络
	*/
	Me3630GetSignal((char*)&NetworkSignalStrength,&Me3630Status ); //获取4G的信号强度
	if(NetworkSignalStrength==99)
	{
		GUICtrData->NetworkSignalStrength=0;
	}else{
		GUICtrData->NetworkSignalStrength=NetworkSignalStrength/20+1;
	}

	//蓝牙的链接状态
	GUICtrData->BleConnection=BL_CONNECTION_STATUS();
	
	//电池电量  0-9 
	/*
		电量显示的代码
	*/
	//工作模式
	GUICtrData->WorkMode=gMySysStatus.bit.WorkMode;
	//工作状态
	
	

}






