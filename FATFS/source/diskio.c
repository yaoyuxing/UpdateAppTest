/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "my_norflash.h"
#include "ff.h"  
/* Definitions of physical drive number for each drive */
#define SD_CARD		3	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_NOR_FLASH 0 






/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	switch (pdrv) {
	case SD_CARD :
	//	stat = SD_Init();

		// translate the reslut code here
    
		return stat;

	case DEV_MMC :
	//	result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_USB :
	//	result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	case  DEV_NOR_FLASH:
	   // stat = BSP_NOR_Init();
	return 0;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here

		//result = SD_ReadDisk(buff, sector, count);

		// translate the reslut code here

		return (DRESULT)result;

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	case DEV_NOR_FLASH:
		 res=(DRESULT)NorFlash_ReadDisk(buff,sector,count);
	   return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res; 

	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here

	//	result = SD_WriteDisk((unsigned char *)buff, sector, count);

		// translate the reslut code here

		return (DRESULT)res;

	case DEV_MMC :
		// translate the arguments here

		// = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

//		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	case DEV_NOR_FLASH:
		res=(DRESULT)NorFlash_WriteDisk((unsigned char *)buff,sector,count);
	return 	 res;
	 
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res; 

	switch (pdrv) {
	case SD_CARD :
//  if(pdrv==SD_CARD)//SD¿¨
//	{
//	    switch(cmd)
//	    {
//		    case CTRL_SYNC:
//				   res = RES_OK; 
//		        break;	 
//		    case GET_SECTOR_SIZE:
//				   *(DWORD*)buff = 512; 
//		        res = RES_OK;
//		        break;	 
//		    case GET_BLOCK_SIZE:
//				   *(WORD*)buff = SDCardInfo.CardBlockSize;
//		        res = RES_OK;
//		        break;	 
//		    case GET_SECTOR_COUNT:
//		        *(DWORD*)buff = SDCardInfo.CardCapacity/512;
//		        res = RES_OK;
//		        break;
//		    default:
//		        res = RES_PARERR;
//		        break;
//	    }
	//}
		// Process of the command for the RAM drive

		return (DRESULT)res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	case DEV_NOR_FLASH:
		  switch(cmd)
	    {
		    case CTRL_SYNC:
				   res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				   *(DWORD*)buff = NOR_FLASH_FS_SECTOR_SIZE; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				   *(WORD*)buff = NOR_FLASH_FS_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = NOR_FLASH_FS_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
		return (DRESULT)res;
	}

	return RES_PARERR;
}

DWORD get_fattime(void)
{
	DWORD   time=0;
//	u32 Hour=0,Min=0,Sec=0,Year=2017-1980,Mon=1,Day=1;
//  DWORD time=0;
//  OS_TICK tick;
//	OS_ERR err;
//	tick=OSTimeGet(&err);
//	tick/=OSCfg_TickRate_Hz;
//	Hour=tick/(60*60);
//	Min=tick%(60*60)/60;
//	Sec=tick%(60*60)%60;
//	time=Sec/2+(Min<<5)+(Hour<<11)+(Day<<15)+(Mon<<21)+(Year<<25);
//  PRINT_COM6("Hour:%d,min:%d,sec:%d\r\n",Hour,Min,Sec);
	return time;
}
