

#include "sys.h"
/*
GUI界面所有的数据
*/
typedef  struct 
{
	u8	NetworkSignalStrength;	//网络信号的强度	0-5 0:没有信号
	u8	BleConnection:1;		//蓝牙连接状态		eGUI_BleStatus	0连接 1没有连接  
	u8	BatteryPower:4;			//电池电量 0-9 		0没有电
	u8	WorkMode:2;				//工作模式 0-2		eGUI_WorkMode 0：基站模式 1：手持模式 2;模式切换
	u8	EquipmentWorkStatus:1;	//设备工作状态 0-1	eEquipmentWorkStatus 0：已经正常工作 1：初始化中 
}stGUICtrData;




/*
工作模式
*/
typedef enum	eGUI_WorkMode
{
	eGUI_BaseStationMode =0,	//基站模式
    eGUI_HandheldMode ,			//手持模式
	eGUI_ModeSwitch				//模式切换
}eGUI_WorkMode;

//蓝牙连接状态
typedef enum	eGUI_BleStatus
{
	
    eGUI_BleConnected=0 ,				//已经连接
	eGUI_BleNoConnection,			//没有连接

}eGUI_BleStatus;
//设备工作状态
typedef enum	eEquipmentWorkStatus
{
	eEquipmentWorkStatussY=0,				//已经正常工作
    eEquipmentWorkStatussN ,				//初始化中
}eEquipmentWorkStatus;



//强度的动态图标  Strength表示信号强度 共有四档信号强度
//Strength :表示信号强值(4)   x：图标起始位置(3) y:坐标的位置(2)
extern void  GUI_4GSignalStrengthDynamic(u8 Strength,u8 x,u8 y);
extern void  GUI_4GSignalStrengthChar();
extern void  GUI_Wireframe(void);
//设备模式切换
extern void GUI_WorkMode(eGUI_WorkMode WorkMode);
//蓝牙连接模式
extern void GUI_BleWorkStatus(eGUI_BleStatus  BleStatus);
//设备工作状态
extern void GUI_EquipmentWorkStatus(eEquipmentWorkStatus EquipmentWorkStatus);
//电量显示
extern void  GUI_BatteryPower(u8 power);


