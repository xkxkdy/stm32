#ifndef __KEY_H
#define __KEY_H 

#include "stm32f10x.h"                  // Device header

void key_Init(void);
uint16_t Get_Key1_Value(void);
uint16_t Get_Key2_Value(void);
uint16_t Get_infrared_val(void);
uint16_t Get_SU03T_val(void);

#endif
