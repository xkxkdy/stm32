#ifndef _DHT11_H_
#define _DHT11_H_

#include "stm32f10x.h"

#define DHT11_PIN			GPIO_Pin_8
#define DHT11_PORT		GPIOA

#define DHT11_HIGH		GPIO_SetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_LOW			GPIO_ResetBits(DHT11_PORT,DHT11_PIN)

#define IN						0
#define OUT						1


u8 DHT11_Init(void);
void DHT11_Mode_Change(u8 mode);
u8 DHT11_Read_Data(u8 *temp,u8 *humi,u8 *temp_point,u8 *humi_point);

#endif

