#include "led.h"


//��ʼ��PB1Ϊ���.��ʹ��ʱ��	    
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();           //����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12; //PB1,0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_LOW;     //����
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_11,GPIO_PIN_SET);	//PB0��1��Ĭ�ϳ�ʼ�������
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_SET);	//PB1��1��Ĭ�ϳ�ʼ�������
}
