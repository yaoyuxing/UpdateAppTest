#include "oled.h"
#include "stdlib.h"
#include "delay.h"
//#include "oledfont.h"
#include "font.h"
		   
u8 OLED_GRAM[128][8];	 

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
#if OLED_MODE==1	//8080����
//ͨ��ƴ�յķ�����OLED���һ��8λ����
//data:Ҫ���������
void OLED_Data_Out(u8 data)
{
	u16 dat=data&0X0F;
	GPIOC->ODR&=~(0XF<<6);		//���6~9
	GPIOC->ODR|=dat<<6;			//D[3:0]-->PC[9:6]
    
    GPIOC->ODR&=~(0X1<<11);		//���11
    GPIOC->ODR|=((data>>4)&0x01)<<11;
    
    GPIOD->ODR&=~(0X1<<3);		//���3
    GPIOD->ODR|=((data>>5)&0x01)<<3;
    
    GPIOB->ODR&=~(0X3<<8);		//���8,9
    GPIOB->ODR|=((data>>6)&0x01)<<8;
    GPIOB->ODR|=((data>>7)&0x01)<<9;
} 
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	OLED_Data_Out(dat);	
 	OLED_RS=cmd;
	OLED_CS(0);	
	OLED_WR(0);	  
	OLED_WR(1);   
	OLED_CS(1);   
	OLED_RS(1);   
} 	    	    
#else
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	OLED_RS(cmd); //д���� 
	OLED_CS(0);	
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK(0);
		if(dat&0x80)OLED_SDIN(1);
		else OLED_SDIN(0);
		OLED_SCLK(1);
		dat<<=1;   
	}		
	OLED_CS(1);		  
	OLED_RS(1);   	  
} 
#endif
	  	  
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
//buffer :���õ��Ǹ��ֿ�

void OLED_ShowChineseChar(u8 x,u8 y,u8 chr,u8 size,u8 mode ,int P_buffer)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=size*2;		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	u8 (*p_buffer)[csize];
	
	p_buffer=(u8(*)[size*2])P_buffer;
    for(t=0;t<csize;t++)
    {   
		temp=p_buffer[chr][t];
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
/*
��ʾ�����ַ���
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
len��Ҫ��ʾ���ַ�������
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
//buffer :���õ��Ǹ��ֿ�

*/
void  OLED_ShowChineseString(u8 x,u8 y,u8 len,u8 size,u8 mode ,int P_buffer)
{
	u8 num=0;
	for(num =0;num<len;num++)
	{
		OLED_ShowChineseChar(x+(size*num),y,num,size,mode ,P_buffer);
	}

}



//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 		 
    GPIO_InitTypeDef  GPIO_Initure;
	
    __HAL_RCC_GPIOA_CLK_ENABLE();   //ʹ��GPIOAʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOBʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();   //ʹ��GPIOFʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();   //ʹ��GPIOHʱ��
#if OLED_MODE==1		//ʹ��8080����ģʽ		
{
	
	//GPIO��ʼ������      
    GPIO_Initure.Pin=GPIO_PIN_15;         //PA15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//�������
    GPIO_Initure.Pull=GPIO_PULLUP;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;   //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   //��ʼ��
	
    //PB3,4,7,8,9
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;	
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);//��ʼ��

    //PC6,7,8,9,11
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;	
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);//��ʼ��	
  
    //PD3
	GPIO_Initure.Pin=GPIO_PIN_3;	
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);//��ʼ��	
	
    //PH8
	GPIO_Initure.Pin=GPIO_PIN_8;	
	HAL_GPIO_Init(GPIOH,&GPIO_Initure);//��ʼ��	
	
 	OLED_WR(1);
	OLED_RD(1); 
}
#else					//ʹ��4��SPI ����ģʽ
	#if OLED_DEVELOPMENT_BOARD
	{
		//GPIO��ʼ������      
		GPIO_Initure.Pin=GPIO_PIN_15;         //PA15
		GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//�������
		GPIO_Initure.Pull=GPIO_PULLUP;        //����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;   //����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);   //��ʼ��
		
		//PB4,7
		GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_7;	
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);//��ʼ��

		//PC6,7
		GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;	
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);//��ʼ��	
	}
	#else  
	{
			//GPIO��ʼ������      
		GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_6;    
		GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//�������
		GPIO_Initure.Pull=GPIO_PULLUP;        //����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;   //����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);   //��ʼ��
		
		GPIO_Initure.Pin=GPIO_PIN_14|GPIO_PIN_7;	  
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);//��ʼ��
		//OLED_PH_EN
		GPIO_Initure.Pin=GPIO_PIN_8;	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);//��ʼ��
		OLED_PH_EN(1);
	}
#endif
	
	OLED_SDIN(1);
	OLED_SCLK(1);
#endif
	OLED_CS(1);
	OLED_RS(1);
	
	OLED_RST(1); 
	OLED_RST(0);
	delay_ms(200);
	OLED_RST(1); 
	delay_ms(200);
#if OLED_DEVELOPMENT_BOARD				  
	OLED_WR_Byte(0xAE,OLED_CMD);
	OLED_WR_Byte(0xD5,OLED_CMD);
	OLED_WR_Byte(0x80,OLED_CMD);
	OLED_WR_Byte(0xA8,OLED_CMD);
	OLED_WR_Byte(0X3F,OLED_CMD);
	OLED_WR_Byte(0xD3,OLED_CMD);
	OLED_WR_Byte(0X00,OLED_CMD);
	OLED_WR_Byte(0x40,OLED_CMD);
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_WR_Byte(0x20,OLED_CMD);
	OLED_WR_Byte(0x02,OLED_CMD);
	OLED_WR_Byte(0xA1,OLED_CMD);
	OLED_WR_Byte(0xC0,OLED_CMD);
	OLED_WR_Byte(0xDA,OLED_CMD);
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0x81,OLED_CMD);
	OLED_WR_Byte(0xEF,OLED_CMD);
	OLED_WR_Byte(0xD9,OLED_CMD);
	OLED_WR_Byte(0xf1,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);
	OLED_WR_Byte(0x30,OLED_CMD);
	OLED_WR_Byte(0xA4,OLED_CMD);
	OLED_WR_Byte(0xA6,OLED_CMD);
	OLED_WR_Byte(0xAF,OLED_CMD);
#else
	OLED_WR_Byte(0xAE,OLED_CMD);
	OLED_WR_Byte(0xD5,OLED_CMD);
	OLED_WR_Byte(0x80,OLED_CMD);
	OLED_WR_Byte(0xA8,OLED_CMD);
	OLED_WR_Byte(0X3F,OLED_CMD);
	OLED_WR_Byte(0xD3,OLED_CMD);
	OLED_WR_Byte(0X00,OLED_CMD);
	OLED_WR_Byte(0x40,OLED_CMD);
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_WR_Byte(0x20,OLED_CMD);
	OLED_WR_Byte(0x02,OLED_CMD);
	OLED_WR_Byte(0xA1,OLED_CMD);
	OLED_WR_Byte(0xC0,OLED_CMD);
	OLED_WR_Byte(0xDA,OLED_CMD);
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0x81,OLED_CMD);
	OLED_WR_Byte(0xEF,OLED_CMD);
	OLED_WR_Byte(0xD9,OLED_CMD);
	OLED_WR_Byte(0xf1,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);
	OLED_WR_Byte(0x30,OLED_CMD);
	OLED_WR_Byte(0xA4,OLED_CMD);
	OLED_WR_Byte(0xA6,OLED_CMD);
	OLED_WR_Byte(0xAF,OLED_CMD);
#endif
	OLED_Clear();
}  