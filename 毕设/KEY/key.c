#include "key.h"

/*
		功能		初始化按键IO口		PB3-控制的LED的变化、PB4-P控制的当前模式
		输入		无
		输出		无
*/
void key_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能GPIOB和IO复用功能模块	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); /* 使用 PA13/14/15 & PB3/4默认配置为JTAG功能需要使能SWD 禁用JTAG*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4	|	GPIO_Pin_14 | GPIO_Pin_15;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);			
	
	GPIO_SetBits(GPIOB,GPIO_Pin_15);

	
}
/*
		功能		判断按键是否按下，以此控制LED的状态		
		输入		无
		输出		0--检测失败、1--按下、2--未按下
*/	
uint16_t Get_Key1_Value(void)
{
	//默认高电平，所以0--开灯、1--熄灯
 	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)
		return 1;
	}
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 1)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 1)
		return 2;
	}
	
	
	return 0;
} 
/*
		功能		判断按键是否按下，改变模式
		输入		无
		输出		0--检测失败、3--按下、4--未按下
*/
uint16_t Get_Key2_Value(void)
{
	//低电平--3，高电平--4
 	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 0)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 0)
		return 3;
	}
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 1)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 1)
		return 4;
	}
	
	
	return 0;
} 
/*
		功能		判断是否有人经过
		输入		无
		输出		0--有人经过，1--无人经过
		说明		有人经过是低电平，无人是高电平
*/
uint16_t Get_infrared_val(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == 0)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == 0)
		{
			return 0;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == 1)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == 1)
		{
			return 1;
		}
	}
	return 2;	
}
/*
		功能		检测声音是否是开灯等
		输入		无
		输出		0--开灯，1--关灯
		说明		开灯是低电平，关灯是高电平
*/
uint16_t Get_SU03T_val(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == 0)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == 0)
		{
			return 0;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == 1)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == 1)
		{
			return 1;
		}
	}
	return 2;
}
