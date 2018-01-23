

#include "sys.h"
#include "rtk.h"
#include "HX_DU1018D.h"
/*
GUI�������е�����
*/
typedef  struct 
{
	u8	NetworkSignalStrength;	//�����źŵ�ǿ��	0-5 0:û���ź�
	u8	BleConnection:1;		//��������״̬		eGUI_BleStatus	0���� 1û������  
	u8	BatteryPower:4;			//��ص��� 0-9 		0û�е�
	u8	WorkMode:2;				//����ģʽ 0-2		eGUI_WorkMode 0����վģʽ 1���ֳ�ģʽ 2;ģʽ�л�
	u8	EquipmentWorkStatus:1;	//�豸����״̬ 0-1	eEquipmentWorkStatus 0���Ѿ��������� 1����ʼ���� 
}stGUICtrData;




/*
����ģʽ
*/
typedef enum	eGUI_WorkMode
{
	eGUI_BaseStationMode =0,	//��վģʽ
    eGUI_HandheldMode ,			//�ֳ�ģʽ
	eGUI_ModeSwitch				//ģʽ�л�
}eGUI_WorkMode;

//��������״̬
typedef enum	eGUI_BleStatus
{
	
    eGUI_BleConnected=0 ,				//�Ѿ�����
	eGUI_BleNoConnection,			//û������

}eGUI_BleStatus;
//�豸����״̬
typedef enum	eEquipmentWorkStatus
{
	eEquipmentWorkStatussY=0,				//�Ѿ���������
    eEquipmentWorkStatussN ,				//��ʼ����
}eEquipmentWorkStatus;



//ǿ�ȵĶ�̬ͼ��  Strength��ʾ�ź�ǿ�� �����ĵ��ź�ǿ��
//Strength :��ʾ�ź�ǿֵ(4)   x��ͼ����ʼλ��(3) y:�����λ��(2)
extern void  GUI_4GSignalStrengthDynamic(u8 Strength,u8 x,u8 y);
extern void  GUI_4GSignalStrengthChar();
extern void  GUI_Wireframe(void);
//�豸ģʽ�л�
extern void GUI_WorkMode(eGUI_WorkMode WorkMode);
//��������ģʽ
extern void GUI_BleWorkStatus(eGUI_BleStatus  BleStatus);
//�豸����״̬
extern void GUI_EquipmentWorkStatus(eEquipmentWorkStatus EquipmentWorkStatus);
//������ʾ
extern void  GUI_BatteryPower(u8 power);
//rtk ״̬��ʾ
#define CUI_RTKStatus_X_START 4
#define CUI_RTKStatus_Y_START 18
extern void CUI_RTKStatus(u8 x,u8 y,stRtkStatus status);
//��̨ͨ����
#define CUI_RadioChannel_X CUI_RTKStatus_X_START
#define CUI_RadioChannel_Y CUI_RTKStatus_Y_START+12
extern void CUI_RadioChannel(u8 x,u8 y,stDu1018dStatus Du1018dStatus);
