#ifndef INTER_FLASH_H
#define INTER_FLASH_H 
 
//#define INTER_FLASH_TEST

#define  APP_SIZE_MAX              (768*1024/2)
#define  BOOT_LOADER_SIZE_MAX      (128*1024)
#define  NV_SIZE_MAX               (128*1024)
#define  INTER_FLASH_BASE          (0X08000000)
#define  BOOT_LOADER_ADDR          (INTER_FLASH_BASE)

#define  SYSTEM_INIT_INFO_ADDR     (BOOT_LOADER_ADDR+BOOT_LOADER_SIZE_MAX)  
#define  USER1_APP_ADDR            (SYSTEM_INIT_INFO_ADDR+NV_SIZE_MAX)
#define  USER2_APP_ADDR            (USER1_APP_ADDR+APP_SIZE_MAX)




 #ifdef INTER_FLASH_TEST
 void interflash_test(void); //start task调用此函数创建测试任务
 #endif
 
 
 
 
 
typedef enum  blockmode
{
	DUAL_BANK_MODE=0,
	SINGLE_BANK_MODE=1
}eBlockModeType;







int InterFlash_EraseSector(unsigned int  Addr,unsigned int offset);
int InterFlash_Program(unsigned int Addr,void* pBuf,unsigned int DataCnt ,unsigned int  DataType);
void InterFlash_Read(unsigned int Addr,void* pBuf,unsigned int DataCnt ,unsigned int  DataType);
int InterFlash_SetBankMode(eBlockModeType bankmode);
eBlockModeType InterFlash_GetBankMode(void);



#endif
