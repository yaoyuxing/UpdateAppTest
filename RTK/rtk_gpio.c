
#include "sys.h"
#include "rtk_gpio.h"
#include "delay.h"
/*RTK��ص�GPIO����*/
//Rtk������ų�ʼ��
void RtkGPIOIint(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	RTK_RESETn_RCC();           		//����GPIOBʱ��
	RTK_P_EN_RCC()	;
	//B��
	GPIO_Initure.Pin=RTK_P_EN_PIN; //
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(RTK_P_EN_GPIO,&GPIO_Initure);
	//RTK״̬���ţ��̶������������
	GPIO_Initure.Pin=RTK_PV_PIN; 
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  //
	HAL_GPIO_Init(RTK_PV_GPIO,&GPIO_Initure);
	//C
	//RTK��λ����
	GPIO_Initure.Pin=RTK_RESETn_PIN; 
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	HAL_GPIO_Init(RTK_RESETn_GPIO,&GPIO_Initure);
	RTK_RESETn(1); //��λ��ƽ����
	RTK_P_EN(1);//rtk��Դ��
}
//RTK��λ
void Rtk_Reset()
{
	RTK_P_EN(1);//rtk��Դ��
	RTK_RESETn(0);
	delay_ms(100);
	RTK_RESETn(1);
}
//Rtk��ʼ��
void RtkDevInit()
{
	RtkGPIOIint();
//	Rtk_Reset();
}



