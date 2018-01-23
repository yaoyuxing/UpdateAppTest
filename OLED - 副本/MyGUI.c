
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "oled.h"
#include "ChineseCharacter.h"
#include "MyGUI.h"


//ͼ����������� 
#define		X_VerticalLineInterval	2//��ǰͼ��ļ��
#define		X_VerticalLineWidth		2//�ߵĿ��
/*�Ͽ�λ�ö���*/
#define		X_START_HEAD  0//д��ʼλ�õ�X������λ����
#define		Y_START_HEAD  2//д��ʼλ�õ�Y�������ƶ�����
//4G�ź�ǿ��
#define		X_SignalStrengthIocnStart	X_START_HEAD //4G�ź�ͼ�꿪ʼλ��
#define		Y_SignalStrengthIocnStart	Y_START_HEAD //4G�ź�ͼ�꿪ʼλ��
#define		SignalStrengthIocnSize		12 //4G�ź�ͼ��ߴ�
#define		X_SignalStrengthIocnEnd		X_SignalStrengthIocnStart+SignalStrengthIocnSize //4G�ź�ͼ�����λ��
#define		X_SignalStrengthCahrStart  	(X_SignalStrengthIocnEnd+4) //4G�źź��ֿ�ʼλ�á��ƶ���
#define		X_SignalStrengthCahrEnd  		(X_SignalStrengthCahrStart+ChCh_4GSignalNUM*12+8) //4G�źź��ֿ�ʼλ�á��ƶ���

//#define		SignalStrengthEnd   (X_SignalStrengthCahrEnd+X_START_HEAD) //4G�źŽ�����ʾ��λ��

//������ʾλ�ö���
#define		BleWorkStatusStart	(X_SignalStrengthCahrEnd+8) //����״̬��ʾ��ʼλ��
#define		X_BleWorkStatusStartIocnSize	(12) //����״̬��ʾͼ��ߴ�

#define		BleWorkStatusCahrStart  	(BleWorkStatusStart+X_BleWorkStatusStartIocnSize+2) //���ֿ�ʼλ��
#define		BleWorkStatusEnd	(BleWorkStatusStart+X_BleWorkStatusStartIocnSize+12*(ChCh_BleConnectedNUM)+8) //����״̬��ʾ����λ��

//���λ�ö���
#define		X_BatteryPowerStart (BleWorkStatusEnd+8)		//���ͼ����ʾX����Ŀ�ʼλ��
#define		Y_BatteryPowerStart	(Y_START_HEAD)		//���ͼ����ʾy����Ŀ�ʼλ��
#define		X_BatteryPowerIocnSize	(12) 				//���ͼ����ʾͼ��ߴ�
#define		X_BatteryPowerEnd 	(X_BatteryPowerStart+X_BatteryPowerIocnSize)		//���ͼ����ʾX����Ľ���λ��
#define		X_BatteryPower_VerticalLine_End 	(X_BatteryPowerEnd+X_VerticalLineWidth+X_VerticalLineInterval)		//���ͼ�����߽���λ��

/*
��λ�ö���
4����y���궨��
*/

#define OUTER_FRAME_Y_1 0
#define OUTER_FRAME_Y_2 15
#define OUTER_FRAME_Y_3 48
#define OUTER_FRAME_Y_4 63

#define OUTER_FRAME_X_1 0
#define OUTER_FRAME_X_2 127
/*
��ʾ�ַ�ͼ���С
*/
#define CHARACTER_ICON_SIZE  12

void  GUI_4GSignalStrengthChar(void);

static void GUI_VerticalLine(u8 x);
//��ص���  power��10����λ  (0��9)
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
��Ļ�߿�
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
�ź�ǿ�ȣ�
һ��4���ź�ǿ�ȣ�
*/

//ǿ�ȵĶ�̬ͼ��  Strength��ʾ�ź�ǿ�� �����ĵ��ź�ǿ��
//Strength :��ʾ�ź�ǿֵ(4)   x��ͼ����ʼλ��(3) y:�����λ��(2)

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
//������12һ�����ص������ x�����λ��  //ͼ����������ʾ  12���� +2���º���+1��Ԥ���ռ䣨�ϣ�=15
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
	
//4g�ź�ǿ�ȵľ�̬��ʾ
void  GUI_4GSignalStrengthChar(void)
{
	GUI_VerticalLine(X_SignalStrengthCahrEnd);
	OLED_ShowChineseString(X_SignalStrengthCahrStart,Y_START_HEAD,ChCh_4GSignalNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_4G_Buffer);
}


//����������״̬
void GUI_BleWorkStatus(eGUI_BleStatus  BleStatus)
{
	switch(BleStatus)
	{
		case eGUI_BleNoConnection: ////û������
			OLED_ShowChineseChar(BleWorkStatusStart,Y_START_HEAD,0,CHARACTER_ICON_SIZE,1 ,(int )ChCh_BleIconStatus);
			OLED_ShowChineseString(BleWorkStatusCahrStart,Y_START_HEAD,ChCh_BleNoConnectedNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_BleNoConnected);
			GUI_VerticalLine(BleWorkStatusEnd);
		break;
		case eGUI_BleConnected:////�Ѿ�����
			OLED_ShowChineseChar(BleWorkStatusStart,Y_START_HEAD,0,CHARACTER_ICON_SIZE,1 ,(int )ChCh_BleIconStatus[1]);
			OLED_ShowChineseString(BleWorkStatusCahrStart,Y_START_HEAD,ChCh_BleConnectedNUM,CHARACTER_ICON_SIZE,1 ,(int)ChCh_BleConnected);
			GUI_VerticalLine(BleWorkStatusEnd);
		break;
		default: break;
	}
}



/*
�±߿���ع��ܶ���
*/
#define		X_VerticalLineGap 14		//s���ߵ�ǰ���϶

#define		X_UnderBorderStart  0   				//�±߿�ͼ����ʾX����Ŀ�ʼλ��
#define		Y_UnderBorderStart  (OUTER_FRAME_Y_3)	//�±߿�ͼ����ʾY����Ŀ�ʼλ��

//�豸ģʽ�л���ʾλ�� 
#define		X_WorkModeStart		14
#define		Y_WorkModeStart		(Y_UnderBorderStart+2)

#define		X_WorkModeVerticalLineStart		(X_WorkModeStart+X_VerticalLineGap+ChCh_ModeSwitchNUM*12)
#define		X_WorkModeVerticalLineEnd		(X_WorkModeVerticalLineStart+X_VerticalLineGap)
//�豸����״̬��ʾλ��

#define		X_EquipmentWorkStatusStart		X_WorkModeVerticalLineEnd
#define		Y_EquipmentWorkStatusStart		(Y_UnderBorderStart+2)

#define		X_EquipmentWorkStatusVerticalLineStart		(X_EquipmentWorkStatusStart+X_VerticalLineGap+3*12) //�豸״̬����ʾ��ʼλ��
#define		X_EquipmentWorkStatusVerticalLineEnd		(X_WorkModeVerticalLineStart+X_VerticalLineGap)           //�豸״̬����ʾ����λ��

//�±߿�ͼ�����������
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
//�豸ģʽ�л�
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

//�豸����״̬
void GUI_EquipmentWorkStatus(eEquipmentWorkStatus EquipmentWorkStatus)
{
#if 0//��ͼ��
	OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusIconNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusIcon);
	switch(EquipmentWorkStatus)
	{
		case eEquipmentWorkStatussY: //��������
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusYNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusY);
		break;
		
		case eEquipmentWorkStatussN: //��ʼ����
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusNNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusN);
		break;
		default: break;
	}
#endif
	
	switch(EquipmentWorkStatus)
	{
		case eEquipmentWorkStatussY: //��������
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusYNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusY);
		break;
		
		case eEquipmentWorkStatussN: //��ʼ����
			OLED_ShowChineseString(X_EquipmentWorkStatusStart,Y_WorkModeStart,ChCh_EquipmentWorkStatusNNum,CHARACTER_ICON_SIZE,1 ,(int)ChCh_EquipmentWorkStatusN);
		break;
		default: break;
	}
}


/*
���������״̬ˢ��
*/
#include "me3630.h"
#include "bluetooth.h"
#include "MySysStatus.h"
void GUI_CtrDataRefresh(stGUICtrData  *GUICtrData)
{
	u8 NetworkSignalStrength=0;//���4G���ź�ǿ��
	//4G�ź�ǿ�ȶ�ȡ
	/*
		0     �C    <0.01%   
		1     �C    0.01% --- 0.1%   
		2     �C    0.1% --- 0.5%   
		3     �C    0.5% --- 1.0%   
		4     �C    1.0% --- 2.0%   
		5     �C    2.0% --- 4.0%   
		6     �C    4.0% --- 8.0%   
		7     �C    ���� 8.0% 
		99    ������
	*/
	Me3630GetSignal((char*)&NetworkSignalStrength,&Me3630Status ); //��ȡ4G���ź�ǿ��
	if(NetworkSignalStrength==99)
	{
		GUICtrData->NetworkSignalStrength=0;
	}else{
		GUICtrData->NetworkSignalStrength=NetworkSignalStrength/20+1;
	}

	//����������״̬
	GUICtrData->BleConnection=BL_CONNECTION_STATUS();
	
	//��ص���  0-9 
	/*
		������ʾ�Ĵ���
	*/
	//����ģʽ
	GUICtrData->WorkMode=gMySysStatus.bit.WorkMode;
	//����״̬
	
	

}






