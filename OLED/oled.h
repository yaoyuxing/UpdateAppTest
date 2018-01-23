#ifndef __OLED_H
#define __OLED_H
#include "sys.h"

//OLEDģʽ����
//0: 4�ߴ���ģʽ  ��ģ���BS1��BS2����GND��
//1: ����8080ģʽ ��ģ���BS1��BS2����VCC��
#define OLED_MODE 	2

#define OLED_DEVELOPMENT_BOARD 	0	

//-----------------OLED�˿ڶ���----------------  	

#if	OLED_DEVELOPMENT_BOARD

	#define OLED_CS(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET))
	#define OLED_RST(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET))
	#define OLED_RS(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)) 
	#define OLED_WR(n)  (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_8,GPIO_PIN_RESET))
	#define OLED_RD(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET))
	 
	//ʹ��4�ߴ��нӿ�ʱʹ�� 
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
	//ʹ��4�ߴ��нӿ�ʱʹ�� 
	#define OLED_SCLK(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET)) //sck PA5
	#define OLED_SDIN(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)) //MISO PA6
	//�ⲿ��Դʹ��
	#define OLED_PH_EN(n) (n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)) //OLED_PH_EN  PA8

#endif

#define OLED_CMD  	0		//д����
#define OLED_DATA 	1		//д����

//OLED�����ú���
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
