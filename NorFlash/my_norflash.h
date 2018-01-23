
#ifndef MY_NOR_FLASH_H
#define MY_NOR_FLASH_H



#define MY_STM32F7XX_USE

#ifdef  MY_STM32F7XX_USE
#include "stm32f7xx.h"  


//#define NOR_FLASH_FS_TEST
#ifdef NOR_FLASH_FS_TEST  
void ceate_fs_test(void);
#endif

// 文件系统扇区和块大小及扇区数量 
#define NOR_FLASH_FS_SECTOR_SIZE    (512)
#define NOR_FLASH_FS_BLOCK_SIZE     (512)
#define NOR_FLASH_FS_SECTOR_COUNT   (32768UL) //1024*1024*16 /512





/* General return codes */
#define ARM_DRIVER_OK                 0 ///< Operation succeeded 
#define ARM_DRIVER_ERROR             -1 ///< Unspecified error
#define ARM_DRIVER_ERROR_BUSY        -2 ///< Driver is busy
#define ARM_DRIVER_ERROR_TIMEOUT     -3 ///< Timeout occurred
#define ARM_DRIVER_ERROR_UNSUPPORTED -4 ///< Operation not supported
#define ARM_DRIVER_ERROR_PARAMETER   -5 ///< Parameter error
#define ARM_DRIVER_ERROR_SPECIFIC    -6 ///< Start of driver specific errors 

/**
\brief General power states
*/ 
typedef enum _ARM_POWER_STATE {
  ARM_POWER_OFF,                        ///< Power off: no operation possible
  ARM_POWER_LOW,                        ///< Low Power mode: retain state, detect and signal wake-up events
  ARM_POWER_FULL                        ///< Power on: full operation at maximum performance
} ARM_POWER_STATE;
 
/**
\brief Flash Sector information
*/
typedef struct _ARM_FLASH_SECTOR {
  uint32_t start;                       ///< Sector Start address
  uint32_t end;                         ///< Sector End address (start+size-1)
} const ARM_FLASH_SECTOR;

/**
\brief Flash information
*/
typedef struct _ARM_FLASH_INFO {
  ARM_FLASH_SECTOR *sector_info;        ///< Sector layout information (NULL=Uniform sectors)
  uint32_t          sector_count;       ///< Number of sectors
  uint32_t          sector_size;        ///< Uniform sector size in bytes (0=sector_info used) 
  uint32_t          page_size;          ///< Optimal programming page size in bytes
  uint32_t          program_unit;       ///< Smallest programmable unit in bytes
  uint8_t           erased_value;       ///< Contents of erased memory (usually 0xFF)
} const ARM_FLASH_INFO;


/**
\brief Flash Status
*/
typedef volatile struct _ARM_FLASH_STATUS {
  uint32_t busy     : 1;                ///< Flash busy flag
  uint32_t error    : 1;                ///< Read/Program/Erase error flag (cleared on start of next operation)
  uint32_t reserved : 30;
} ARM_FLASH_STATUS;


/****** Flash Event *****/
#define ARM_FLASH_EVENT_READY           (1UL << 0)  ///< Flash Ready
#define ARM_FLASH_EVENT_ERROR           (1UL << 1)  ///< Read/Program/Erase Error


typedef enum my_nor_flash_status
{
	NOR_STATUS_OK =0,
	NOR_STATUS_ERROR
}eMyNorFlashStatusType; 
typedef void (*ARM_Flash_SignalEvent_t) (uint32_t event);    ///< Pointer to \ref ARM_Flash_SignalEvent : Signal Flash Event.


/**
\brief Flash Driver Capabilities.
*/
typedef struct _ARM_FLASH_CAPABILITIES {
  uint32_t event_ready  : 1;            ///< Signal Flash Ready event
  uint32_t data_width   : 2;            ///< Data width: 0=8-bit, 1=16-bit, 2=32-bit
  uint32_t erase_chip   : 1;            ///< Supports EraseChip operation
  uint32_t reserved     : 28;           ///< Reserved (must be zero)
} ARM_FLASH_CAPABILITIES;




 

uint8_t BSP_NOR_Init(void); 
unsigned char  NorFlash_WriteDisk( unsigned char *buf,unsigned int sector,unsigned int cnt);
unsigned char  NorFlash_ReadDisk(unsigned char *buf,unsigned int sector,unsigned int cnt);
void  MPU_ConfigNOR(void);

#endif




#endif
