
#include  "my_norflash.h"  
#include "stm32f7xx_hal.h" 
#include "string.h"
#include <stdbool.h>


/* how to use this driver 
  1、初始化norflash 函数BSP_NOR_Init() 包含头文件 my_norflash.h
		 依赖:
			a、 MPU_ConfigNOR();
			b、 Cache_Enable();                 //打开L1-Cache 
			c、 Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
	2、直接使用norflash 读写可调用NorFlash_WriteDisk、NorFlash_ReadDisk进行读写。
  3、使用fatfs文件系统包含头文件 ff.h   即可使用文件系统api操作
  4、调试fs 可在my_norflash.h 定义NOR_FLASH_FS_TEST 在start_task调用ceate_fs_test函数 
*/



//bsp init part

//#define NOR_MEMORY_WIDTH    FMC_NORSRAM_MEM_BUS_WIDTH_8  
#define NOR_MEMORY_WIDTH    FMC_NORSRAM_MEM_BUS_WIDTH_16


#define NOR_BURSTACCESS    FMC_BURST_ACCESS_MODE_DISABLE  
//#define NOR_BURSTACCESS    FMC_BURST_ACCESS_MODE_ENABLE

#define NOR_WRITEBURST    FMC_WRITE_BURST_DISABLE  
// #define NOR_WRITEBURST   FMC_WRITE_BURST_ENABLE 
 
//#define NOR_CONTINUOUS_CLOCK    FMC_CONTINUOUS_CLOCK_SYNC_ONLY 
 #define NOR_CONTINUOUS_CLOCK     FMC_CONTINUOUS_CLOCK_SYNC_ASYNC 

/* NOR operations Timeout definitions */
#define BLOCKERASE_TIMEOUT   ((uint32_t)0x00A00000)  /* NOR block erase timeout */
#define CHIPERASE_TIMEOUT    ((uint32_t)0x30000000)  /* NOR chip erase timeout  */ 
#define PROGRAM_TIMEOUT      ((uint32_t)0x00004400)  /* NOR program timeout     */ 

/* NOR Ready/Busy signal GPIO definitions */
#define NOR_READY_BUSY_PIN    GPIO_PIN_6 
#define NOR_READY_BUSY_GPIO   GPIOC
#define NOR_READY_STATE       GPIO_PIN_SET
#define NOR_BUSY_STATE        GPIO_PIN_RESET  

//驱动部分
 
#define  FLASH_ADDR              0x60000000 
#define  NOR_FLASH_WRITE_BUFF_SIZE   (128*1024/2)    //缓冲区大小
#define  NOR_FLASH_ADR(sector)       (FLASH_SECTOR_SIZE*(sector)) //扇区地址

#define  ARM_FLASH_SECTOR_INFO   (addr,size) { (addr), (addr)+(size)-1 }
#define  FLASH_SECTOR_COUNT      ((uint32_t)128)    /* Number of sectors */
#define  FLASH_SECTOR_SIZE       ((uint32_t)131072) /* Uniform sectors of 128kB */
#define  FLASH_PAGE_SIZE         ((uint32_t)2)      /* Programming page size in bytes */
#define  FLASH_PROGRAM_UNIT      ((uint32_t)2)      /* Smallest programmable unit in bytes */
#define  FLASH_ERASED_VALUE      ((uint8_t)0xFF)    /* Contents of erased memory */ 

/* 16-bit Memory Bus Access Macro */
#define M16(addr)               (*((volatile uint16_t *) (addr)))

/* Flash Base Address */
#define BASE_ADDR               ((uint32_t)FLASH_ADDR)

/* Flash Commands */
#define CMD_RESET               ((uint16_t)0xF0)
#define CMD_ERASE               ((uint16_t)0x80)
#define CMD_ERASE_CHIP          ((uint16_t)0x10)
#define CMD_ERASE_SECTOR        ((uint16_t)0x30)
#define CMD_PROGRAM             ((uint16_t)0xA0)

/* Flash Status Register bits */
#define DQ6                     ((uint32_t)(1UL << 6))
#define DQ5                     ((uint32_t)(1UL << 5))

/* Flash Driver Flags */
#define FLASH_INIT              (0x01U)
#define FLASH_POWER             (0x02U)

/* Sector Information */
#ifdef FLASH_SECTORS
static ARM_FLASH_SECTOR FLASH_SECTOR_INFO[FLASH_SECTOR_COUNT] = FLASH_SECTORS;
#else
#define FLASH_SECTOR_INFO NULL
#endif

 
  
unsigned short cgu16NorFlash_WriteBuff[NOR_FLASH_WRITE_BUFF_SIZE]={0};//读取擦拭前128k数据缓冲区 malloc ps:malloc方式 内存碎片可能导致多次申请失败

static  void  BSP_NOR_MspInit(NOR_HandleTypeDef  *hnor, void *Params); 
static  int   GetStartADR(unsigned int adr);
/* Flash Driver Control Block */
static bool DQ6_Polling (uint32_t addr);
 ARM_FLASH_CAPABILITIES GetCapabilities (void);
 int32_t Initialize (ARM_Flash_SignalEvent_t cb_event);
 int32_t Uninitialize (void) ;
 int32_t PowerControl (ARM_POWER_STATE state);
static int32_t ReadData (uint32_t addr, void *data, uint32_t cnt);
static int32_t ProgramData (uint32_t addr, const void *data, uint32_t cnt);
static int32_t EraseSector (uint32_t addr);
 int32_t EraseChip (void) ;
 ARM_FLASH_STATUS GetStatus (void);
 ARM_FLASH_INFO * GetInfo (void);


#ifdef  NOR_FLASH_FS_TEST
#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"
 FIL file[1];
 FATFS fatfs[1];
 FILINFO fileinfo[1];
 DIR    filedir[1];

 u8 res=0; 
 u8 work[4096]={0}; 
 u8  readbuf[512];
 u32 len ;
TaskHandle_t  FS_TestTask_Handler;
#define  FS_TEST_TASK_PRIO   5
#define  FS_TEST_STACK_SIZE   1024
 static void fs_test_task(void *param);
void ceate_fs_test(void)   //start task调用此函数创建测试任务
{
		xTaskCreate((TaskFunction_t )fs_test_task,             
							(const char*    )"FS Test Task",           
							(uint16_t       )FS_TEST_STACK_SIZE,        
							(void*          )NULL,                  
							(UBaseType_t    )FS_TEST_TASK_PRIO,        
							(TaskHandle_t*  )&FS_TestTask_Handler); 
}
static void fs_test_task(void *param)
{ 

	
	#if 0
	#define  ADR   (128*1024*0)
	 NorFlash_ReadDisk(readbuf,ADR,1);
	 NorFlash_WriteDisk("123456789",ADR ,1);
	 NorFlash_ReadDisk(readbuf,ADR,1);
	#else 
	
	res=	f_mount (&fatfs[0],"0:",0 );	
//	res=f_mkfs("0:",FM_FAT|FM_SFD,0,work,sizeof work );
	res=f_mkdir("0:test.txt");
  if(res==FR_NO_FILESYSTEM)
  	res=f_mkfs("0:",FM_FAT|FM_SFD,0,work,sizeof work );
	else f_unlink("0:test.txt");
	// NorFlash_ReadDisk(readbuf,0,1); 
	res=f_open(file,"0:123.bin",FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 
	res=f_write(file,"jk1234556",sizeof"abcdefgh",&len);
	res=f_close(file);
	res=f_open(file,"0:123.bin",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	res=f_read(file,readbuf,10,&len);
	res=f_close(file);
#endif	
  while(1)
	{}		
}
#endif
/**
  * @brief  Initializes the NOR device.
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Init(void)
{ 
	NOR_HandleTypeDef norHandle;
  FMC_NORSRAM_TimingTypeDef Timing;
 // NOR_IDTypeDef  Id_struct; 
  uint8_t nor_status = NOR_STATUS_ERROR;
  ARM_Flash_SignalEvent_t even;
//	uint8_t cnt =0;
  norHandle.Instance  = FMC_NORSRAM_DEVICE;
  norHandle.Extended  = FMC_NORSRAM_EXTENDED_DEVICE;
  /* NOR device configuration */
  /* Timing configuration derived from system clock (up to 216Mhz)
     for 108Mhz as NOR clock frequency */  
  Timing.AddressSetupTime      =4;
  Timing.AddressHoldTime       = 3;
  Timing.DataSetupTime         = 8;
  Timing.BusTurnAroundDuration = 1;
  Timing.CLKDivision           = 216/40; //216Mhz/16=
  Timing.DataLatency           = 2;
  Timing.AccessMode            = FMC_ACCESS_MODE_A; 
  norHandle.Init.NSBank             = FMC_NORSRAM_BANK1;
  norHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  norHandle.Init.MemoryType         = FMC_MEMORY_TYPE_NOR;
  norHandle.Init.MemoryDataWidth    = NOR_MEMORY_WIDTH;
  norHandle.Init.BurstAccessMode    = NOR_BURSTACCESS;
  norHandle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  norHandle.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;
  norHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  norHandle.Init.WaitSignal         = FMC_WAIT_SIGNAL_ENABLE;
  norHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  norHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_ENABLE;
  norHandle.Init.WriteBurst         = NOR_WRITEBURST;
  norHandle.Init.ContinuousClock    = NOR_CONTINUOUS_CLOCK;
  
  	  /* NOR controller initialization */
  BSP_NOR_MspInit(&norHandle, NULL); /* __weak function can be rewritten by the application */
  if(HAL_NOR_Init(&norHandle, &Timing, &Timing) != HAL_OK)
  {
    nor_status = NOR_STATUS_ERROR;
  }
  else
  {
    nor_status = NOR_STATUS_OK;
  }  
	Initialize(even);
	
//			  ReadData (0,cgu16NorFlash_WriteBuff,NOR_FLASH_WRITE_BUFF_SIZE);
//	    while( GetStatus().busy==1 );
//		  EraseSector(0); 
//	    while( GetStatus().busy==1 ); 
//	    ProgramData(0,cgu16NorFlash_WriteBuff,NOR_FLASH_WRITE_BUFF_SIZE);
//		  while( GetStatus().busy ==1);
	
	
  return nor_status;
}


void MPU_ConfigNOR(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* Disable the MPU */
  HAL_MPU_Disable(); 
  /* Configure the MPU attributes as device for NOR */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x60000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  Initializes the NOR MSP.
  * @param  norHandle: NOR handle
  * @param  Params    
  * @retval None
  */
static void BSP_NOR_MspInit(NOR_HandleTypeDef *norHandle, void *Params)
{
  GPIO_InitTypeDef gpio_init_structure;
  
  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();  
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  
  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  
    /* GPIOC configuration */
  gpio_init_structure.Alternate = GPIO_AF9_FMC;
  gpio_init_structure.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOC, &gpio_init_structure);
  
  /* GPIOD configuration */
  gpio_init_structure.Alternate = GPIO_AF12_FMC;  
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5|\
                              GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |\
                              GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);
    
  /* GPIOE configuration */  
  gpio_init_structure.Pin   = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6   |\
                              GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |\
                              GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);
  
  /* GPIOF configuration */  
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4     |\
                              GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &gpio_init_structure);
  
  /* GPIOG configuration */  
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4     |\
                              GPIO_PIN_5;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);
}
 



/* Flash Information */
static ARM_FLASH_INFO FlashInfo = {
  FLASH_SECTOR_INFO,
  FLASH_SECTOR_COUNT,
  FLASH_SECTOR_SIZE,
  FLASH_PAGE_SIZE,
  FLASH_PROGRAM_UNIT,
  FLASH_ERASED_VALUE
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0};
static uint8_t Flags;



/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
  0U,   /* event_ready */
  1U,   /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
  1U    /* erase_chip */
};


/* Check if Program/Erase completed */
               // Device header
static bool DQ6_Polling (uint32_t addr) {
  uint32_t fsreg;
  uint32_t dqold;

  fsreg = M16(addr);
  do {
    dqold = fsreg & DQ6;
    fsreg = M16(addr);
    if ((fsreg & DQ6) == dqold) {
      return true;              /* Done */
    }
  } while ((fsreg & DQ5) != DQ5);
  fsreg = M16(addr);
  dqold = fsreg & DQ6;
  fsreg = M16(addr);
  if ((fsreg & DQ6) == dqold) {
    return true;                /* Done */
  }
  M16(addr) = CMD_RESET;        /* Reset Flash Device */
  return false;                 /* Error */
}



/**
  \fn          ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_FLASH_CAPABILITIES
*/
 ARM_FLASH_CAPABILITIES GetCapabilities (void) {
  return DriverCapabilities;
}

/**
  \fn          int32_t ARM_Flash_Initialize (ARM_Flash_SignalEvent_t cb_event)
  \brief       Initialize the Flash Interface.
  \param[in]   cb_event  Pointer to \ref ARM_Flash_SignalEvent
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_Flash_SignalEvent_t cb_event) {
  Flags = FLASH_INIT;
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_Flash_Uninitialize (void)
  \brief       De-initialize the Flash Interface.
  \return      \ref execution_status
*/
 int32_t Uninitialize (void) {
  Flags = 0U;
  return ARM_DRIVER_OK;
} 

/**
  \fn          int32_t ARM_Flash_PowerControl (ARM_POWER_STATE state)
  \brief       Control the Flash interface power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
 int32_t PowerControl (ARM_POWER_STATE state) {

  switch (state) {
      case ARM_POWER_OFF:
        Flags &= ~FLASH_POWER;

        M16(BASE_ADDR) = CMD_RESET;

        FlashStatus.busy  = 0U;
        FlashStatus.error = 0U;
      break;

    case ARM_POWER_FULL:
      if ((Flags & FLASH_INIT) == 0U) {
        return ARM_DRIVER_ERROR;
      }

      if ((Flags & FLASH_POWER) == 0U) {
        M16(BASE_ADDR) = CMD_RESET;

        FlashStatus.busy  = 0U;
        FlashStatus.error = 0U;

        Flags |= FLASH_POWER;
      }
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    default:
      return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_Flash_ReadData (uint32_t addr, void *data, uint32_t cnt)
  \brief       Read data from Flash.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or \ref execution_status
*/
static int32_t ReadData (uint32_t addr, void *data, uint32_t cnt) {
  uint16_t *mem;
  uint32_t  n;

  if (((addr & 1U) != 0U) || (data == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (FlashStatus.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  FlashStatus.error = 0U;

  addr += BASE_ADDR;
  mem = data;
  for (n = cnt; n; n--) {
    *mem  = M16(addr);
    mem  += 1U;
    addr += 2U;
  }

  return (int32_t)cnt;
}

/**
  \fn          int32_t ARM_Flash_ProgramData (uint32_t addr, const void *data, uint32_t cnt)
  \brief       Program data to Flash.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash.
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or \ref execution_status
*/
static int32_t ProgramData (uint32_t addr, const void *data, uint32_t cnt) {
  const uint16_t *mem;
        uint32_t  n;

  if (((addr & 1U) != 0U) || (data == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (FlashStatus.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  if (cnt == 0U) { return 0; }

  FlashStatus.busy  = 1U;
  FlashStatus.error = 0U;

  addr += BASE_ADDR;
  mem = data;
  for (n = cnt; n; n--) {
    M16(BASE_ADDR + (0x555UL << 1)) = 0xAAU;
    M16(BASE_ADDR + (0x2AAUL << 1)) = 0x55U;
    M16(BASE_ADDR + (0x555UL << 1)) = CMD_PROGRAM;
    M16(addr) = *mem;
    mem  += 1U;
    addr += 2U;
    if (n > 1U) {
      if (!DQ6_Polling(BASE_ADDR)) {
        FlashStatus.busy  = 0U;
        FlashStatus.error = 1U;
        return ARM_DRIVER_ERROR;
      }
    }
  }

  return ((int32_t)cnt - 1);
}

/**
  \fn          int32_t ARM_Flash_EraseSector (uint32_t addr)
  \brief       Erase Flash Sector.
  \param[in]   addr  Sector address
  \return      \ref execution_status
*/
static int32_t EraseSector (uint32_t addr) {

  if (FlashStatus.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  FlashStatus.busy  = 1U;
  FlashStatus.error = 0U;

  M16(BASE_ADDR + (0x555UL << 1)) = 0xAAU;
  M16(BASE_ADDR + (0x2AAUL << 1)) = 0x55U;
  M16(BASE_ADDR + (0x555UL << 1)) = CMD_ERASE;
  M16(BASE_ADDR + (0x555UL << 1)) = 0xAAU;
  M16(BASE_ADDR + (0x2AAUL << 1)) = 0x55U;
  M16(BASE_ADDR +  addr)          = CMD_ERASE_SECTOR;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_Flash_EraseChip (void)
  \brief       Erase complete Flash.
               Optional function for faster full chip erase.
  \return      \ref execution_status
*/
 int32_t EraseChip (void) {

  if (FlashStatus.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  FlashStatus.busy  = 1U;
  FlashStatus.error = 0U;

  M16(BASE_ADDR + (0x555UL << 1)) = 0xAAU;
  M16(BASE_ADDR + (0x2AAUL << 1)) = 0x55U;
  M16(BASE_ADDR + (0x555UL << 1)) = CMD_ERASE;
  M16(BASE_ADDR + (0x555UL << 1)) = 0xAAU;
  M16(BASE_ADDR + (0x2AAUL << 1)) = 0x55U;
  M16(BASE_ADDR + (0x555UL << 1)) = CMD_ERASE_CHIP;

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_FLASH_STATUS ARM_Flash_GetStatus (void)
  \brief       Get Flash status.
  \return      Flash status \ref ARM_FLASH_STATUS
*/
static ARM_FLASH_STATUS GetStatus (void) {
  uint32_t fsreg;
  uint32_t dqold;

  if (FlashStatus.busy) {
    fsreg = M16(BASE_ADDR);
    dqold = fsreg & DQ6;
    fsreg = M16(BASE_ADDR);
    if ((fsreg & DQ6) == dqold) {
      FlashStatus.busy = 0U;
    } else {
      if (fsreg & DQ5) {
        M16(BASE_ADDR) = CMD_RESET;
        FlashStatus.busy  = 0U;
        FlashStatus.error = 1U;
      }
    }
  }
  return FlashStatus;
}

/**
  \fn          ARM_FLASH_INFO * ARM_Flash_GetInfo (void)
  \brief       Get Flash information.
  \return      Pointer to Flash information \ref ARM_FLASH_INFO
*/
 ARM_FLASH_INFO * GetInfo (void) {
  return &FlashInfo;
}




static int  GetStartADR(unsigned int adr)
{
	int cnt =0;
	for(cnt=0;cnt <FLASH_SECTOR_COUNT;cnt++)
	{
		if(NOR_FLASH_ADR(cnt)>adr*NOR_FLASH_FS_SECTOR_SIZE)
		{
			return NOR_FLASH_ADR(cnt-1);
		}
	}
		return -1;
} 
//读norflash
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;	
unsigned char  NorFlash_ReadDisk(unsigned char *buf,unsigned int sector,unsigned int cnt)
{
//	  taskENTER_CRITICAL();           //进入临界区
   int  sta=HAL_NOR_STATUS_SUCCESS;   
	  ReadData (sector*NOR_FLASH_FS_SECTOR_SIZE,buf,cnt*NOR_FLASH_FS_BLOCK_SIZE/2);
	  while( GetStatus().busy==1 );
	//    taskEXIT_CRITICAL();            //退出临界区 
	return sta;
}


//写norflash
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;	
unsigned char NorFlash_WriteDisk( unsigned char *buf,unsigned int sector,unsigned int cnt)
{
	 int  sta=HAL_NOR_STATUS_SUCCESS;   
   int  StartADR=0; 
	 int  offset=0;
	 StartADR =GetStartADR(sector);
	 offset=sector*NOR_FLASH_FS_SECTOR_SIZE-StartADR;
	 if(StartADR!=-1)
	 {
		// 	  taskENTER_CRITICAL();           //进入临界区
		  ReadData (StartADR,cgu16NorFlash_WriteBuff,NOR_FLASH_WRITE_BUFF_SIZE);
	    while( GetStatus().busy==1 );
		  memcpy((void*)&cgu16NorFlash_WriteBuff[offset/2],buf,cnt*NOR_FLASH_FS_BLOCK_SIZE);
		  EraseSector(StartADR); 
	    while( GetStatus().busy==1 ); 
	    ProgramData(StartADR,cgu16NorFlash_WriteBuff,NOR_FLASH_WRITE_BUFF_SIZE);
		  while( GetStatus().busy ==1);
		  sta=HAL_NOR_STATUS_SUCCESS;
		 	//   taskEXIT_CRITICAL();            //退出临界区 
	 }
	 else 
	 {
		 sta= NOR_STATUS_ERROR;//返回错误
	 }
	return sta;
}



 
