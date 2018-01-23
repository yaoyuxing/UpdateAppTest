#ifndef  BOOT_LOADER_H
#define BOOT_LOADER_H

#include "ble_update.h"
#include "ymodem.h"
#define  MODULE_NAME_LEN_MAX       100
#define  MODULE_HARD_VS_LEN_MAX    10
#define  MODULE_FIRE_VS_LEN_MAX    10
#define  MODULE_TIME_LEN_MAX       16

#define  APP_PATH                  "0:App.bin"
#define  APP_BUF_MAX               (1024*150)
//升级收发字符
#define  UPDATE_SUCCEED_ACK_BYTE          0X03
#define  UPDATE_FAILED_ACK_BYTE           0X04
//升级bin分隔字符串
#define  UPDATE_APP_SEPARATE_STR       "#APP SEPARATE LINE#"


typedef  enum 
{
    BOOT_NO_ERR,
	WRITE_ERR,
	STOP_BYTE_ERR,
	RAM_ADDR_ERR,
    WRITE_TO_FLASH_ERR,
	WRITE_TO_SD_ERR,
	READ_SD_ERR,
	APP_NO_END,
	APP_SIZE_ERR,
	NO_NEW_APP,
	APP_ERR
}eUpdateErrType;

typedef  enum 
{
	NO_UPDATE=0,
    START_UPDATE,
	USR1_START_UPDATE,
	USR2_START_UPDATE,
	USR1_UPDATE_FINISH,
	USR2_UPDATE_FINISH,
	RUN_OLD_APP,
}eUpdateStatusType;



#define  BOOT_LOADER_SYS_INFO_SIZE           (sizeof (stUpdateInfoType))


typedef struct fireinfo
{
	char ModuleName[MODULE_NAME_LEN_MAX];
	char HardVersion[MODULE_HARD_VS_LEN_MAX];
	char FireVersion[MODULE_FIRE_VS_LEN_MAX];
	char  Time[MODULE_TIME_LEN_MAX]; 
}stFireInfoType;

typedef struct updateinfo
{ 
    stFireInfoType     stFireInfo;
	unsigned  int      unCurrentAppADDR;
	unsigned  int      unCurrentAppSize;
	eUpdateStatusType  eAppUpdateStatus; 
	stBleInfoType      stBleInfo;
}stUpdateInfoType;

/**************************                       升级部分函数及变量                      ******************************/
typedef void( *pFunType)(void);
extern unsigned char gucAppBuf[APP_BUF_MAX]; 
extern volatile stUpdateInfoType  gstUpdate;
eUpdateErrType CheckAppBin(unsigned int *AppBuf);
void SaveAppBackup(char *pData,unsigned int len );
void RunAPP(unsigned int jumpAdr);
void CheckUpdateStatus(void);
void UpdateInit_App(void);
char * pGetAppBin(char *pBinData,unsigned int len,unsigned int Addr);
/*********************************************************************************************************************/






/**************************                       常用函数                               ******************************/
/*
* 在给定buf中找字符串
*/
int  findstr(unsigned char  *rx_buf,unsigned int rx_counter, char *str);
unsigned int bytepos(char *str,char byte);
#define  bt_crc16              Cal_CRC16








/*********************************************************************************************************************/
#endif
