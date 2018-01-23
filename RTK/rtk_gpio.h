#ifndef __RTK_GPIOH__
#define __RTK_GPIOH__
//rtk��λ C4
#define RTK_RESETn_RCC()	__HAL_RCC_GPIOC_CLK_ENABLE()	//4
#define	RTK_RESETn_GPIO		GPIOC
#define RTK_RESETn_PIN		GPIO_PIN_4
//��Դʹ��  B15
#define RTK_P_EN_RCC()		__HAL_RCC_GPIOB_CLK_ENABLE() //15
#define	RTK_P_EN_GPIO		GPIOB
#define RTK_P_EN_PIN		GPIO_PIN_15
//B4
#define	RTK_PV_GPIO			GPIOB
#define RTK_PV_PIN			GPIO_PIN_4

#define RTK_PV()			HAL_GPIO_ReadPin(RTK_PV_GPIO,RTK_PV_PIN) //1״̬����4   0
#define RTK_RESETn(n)  		(n?HAL_GPIO_WritePin(RTK_RESETn_GPIO,RTK_RESETn_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RTK_RESETn_GPIO,RTK_RESETn_PIN,GPIO_PIN_RESET))
#define RTK_P_EN(n)  		(n?HAL_GPIO_WritePin(RTK_P_EN_GPIO,RTK_P_EN_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RTK_P_EN_GPIO,RTK_P_EN_PIN,GPIO_PIN_RESET))


//Rtk������ų�ʼ��
void RtkGPIOIint(void);
//RTK��λ
void Rtk_Reset();
//Rtk��ʼ��
void RtkDevInit();
#endif
