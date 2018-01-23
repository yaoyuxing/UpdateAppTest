
#include "sys.h"
#include "rtk_gpio.h"
#include "delay.h"
/*RTK相关的GPIO配置*/
//Rtk相关引脚初始化
void RtkGPIOIint(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	RTK_RESETn_RCC();           		//开启GPIOB时钟
	RTK_P_EN_RCC()	;
	//B口
	GPIO_Initure.Pin=RTK_P_EN_PIN; //
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(RTK_P_EN_GPIO,&GPIO_Initure);
	//RTK状态引脚，固定解后引脚拉高
	GPIO_Initure.Pin=RTK_PV_PIN; 
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  //
	HAL_GPIO_Init(RTK_PV_GPIO,&GPIO_Initure);
	//C
	//RTK复位配置
	GPIO_Initure.Pin=RTK_RESETn_PIN; 
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	HAL_GPIO_Init(RTK_RESETn_GPIO,&GPIO_Initure);
	RTK_RESETn(1); //复位电平拉高
	RTK_P_EN(1);//rtk电源打开
}
//RTK复位
void Rtk_Reset()
{
	RTK_P_EN(1);//rtk电源打开
	RTK_RESETn(0);
	delay_ms(100);
	RTK_RESETn(1);
}
//Rtk初始化
void RtkDevInit()
{
	RtkGPIOIint();
//	Rtk_Reset();
}



