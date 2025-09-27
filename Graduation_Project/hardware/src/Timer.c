#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "stdio.h"
#include "led.h"
#include "usart.h"
#include "key.h"

char OledBuf[64];
char String[100];

uint8_t LED_Status = 0;		//不去参与任何运算，只是记录LED的当前状态被返回云台

extern uint8_t su03t_status;
extern uint8_t infrared_status;
extern uint16_t Light_ADValue;
extern float Voltage;
extern uint16_t LedFlag;
extern uint8_t auto_manual_mode;		//onenet.c文件下
extern uint8_t key_val;


/*
		功能		初始化定时器2
		输入		arr：自动重装值,psc：时钟预分频数
		输出		无
*/	
void Timer2_Init(u16 arr,u16 psc)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr;			//自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;		//设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//使能或者失能指定的TIM中断
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ通道被使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3级
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

/*
		功能		定时器2中断处理函数，用来按一定频率刷新OLED
		输入		无
		输出		无
*/	
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{	
		
		OLED_ShowString(1, 1, "      ^_^    ");
		
		if(auto_manual_mode == 2)
		{
			OLED_ShowString(2, 1, "Manual");			
		}			
		else if(auto_manual_mode == 3)
		{
			OLED_ShowString(2, 1, "Auto  ");
		}
			
		if(LED_Status == 0)
		{
			OLED_ShowString(3, 1, "LED:OFF");
		}
		else if(LED_Status == 1)
		{
			OLED_ShowString(3, 1, "LED:ON ");
		}
		
//		sprintf(OledBuf,"Light:%.1f Lx",Voltage);	
//		OLED_ShowString(3, 1,OledBuf);	
			
		sprintf(OledBuf,"Light:%d",Light_ADValue);	
		OLED_ShowString(4, 1, OledBuf);
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
/*
		功能		初始化定时器3
		输入		arr：自动重装值,psc：时钟预分频数
		输出		无
*/	
void Timer3_Init(u16 arr,u16 psc)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr;			//自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;		//设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//使能或者失能指定的TIM中断
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ通道被使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3级
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
}

/*
		功能		定时器3中断处理函数，一定的周期对灯的状态进行再判断
		输入		无
		输出		无
*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{	
		if(auto_manual_mode == 2)		//手动模式下
		{	
			if(key_val == 0)
			{	
				LED_Status = 0;
				LED_OFF;
				
			}
			else if(key_val == 1)
			{
				LED_Status = 1;
				LED_ON;
			}
		}
		//判断在光线充足的时候不管是否有人都不开，当光线暗下后，有人经过时才开的灯
		else if(auto_manual_mode == 3)
		{
			//光线暗											有人经过									
			if( Light_ADValue > 2000 && infrared_status == 0 )
			{
					LED_Status = 1;
					LED_ON;
			}
			else if(Light_ADValue < 2000)
			{
				if(su03t_status == 0)
				{
					LED_Status = 1;
					LED_ON;
				}
				else if(su03t_status == 1)
				{
					LED_Status = 0;
					LED_OFF;
				}
			}
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

