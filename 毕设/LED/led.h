#ifndef __LED_H
#define __LED_H 

#include "stm32f10x.h"                  // Device header

#define LED_OFF  	GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define LED_ON  	GPIO_ResetBits(GPIOB,GPIO_Pin_13)

//#define LED2_OFF  GPIO_SetBits(GPIOB,GPIO_Pin_6)
//#define LED2_ON  	GPIO_ResetBits(GPIOB,GPIO_Pin_6)

//#define LED3_OFF  GPIO_SetBits(GPIOB,GPIO_Pin_7)
//#define LED3_ON  	GPIO_ResetBits(GPIOB,GPIO_Pin_7)

void LED_Init(void);

#endif




