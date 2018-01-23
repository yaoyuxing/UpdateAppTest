#ifndef __OLED_H
#define __OLED_H
#include "sys.h"

//OLED模式设置
//0: 4线串行模式  （模块的BS1，BS2均接GND）
//1: 并行8080模式 （模块的BS1，BS2均接VCC）
#define OLED_MODE 	2

#define OLED_DEVELOPMENT_BOARD 	0	

//-----------------OLED端口定义----------------  	

#if	OLED_DEVELOPMENT_BOARD

	#define OLED_CS(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET))
	#define OLED_RST(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET))
	#define OLED_RS(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)) 
	#define OLED_WR(n)  (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_RESET))
	#define OLED_RD(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET))
	 
	//使用4线串行接口时使用 
	#define OLED_SCLK(n) (n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET)) 
	#define OLED_SDIN(n) (n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET))

#else

	#define OLED_CS(n)  (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET))	//CS  SPI1_NSS  PA4
	#define OLED_RST(n) (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET))//RST PB14
	#define OLED_RS(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)) //DC  PB7
	#if OLED_MODE==1
		#define OLED_WR(n)  (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_RESET))  
		#define OLED_RD(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET))
	#endif 
	//使用4线串行接口时使用 
	#define OLED_SCLK(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET)) //sck PA5
	#define OLED_SDIN(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)) //MISO PA6
	//外部电源使能
	#define OLED_PH_EN(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)) //OLED_PH_EN  PA8

#endif

#define OLED_CMD  	0		//写命令
#define OLED_DATA 	1		//写数据

//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);
void  OLED_ShowChineseString(u8 x,u8 y,u8 len,u8 size,u8 mode ,int P_buffer);
void OLED_ShowChineseChar(u8 x,u8 y,u8 chr,u8 size,u8 mode ,int P_buffer);
extern int oled_test(void);
#endif
