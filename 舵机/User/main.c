#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Key.h"
#include "PWM.h"
#include "LED.h"

uint8_t KeyNum;
float Angle = 0;

int main(void)
{
	Servo_Init();

	Servo_SetAngle(90);
	
	while(1)
	{
		Servo_SetAngle(40);
		Delay_ms(1000);
		Servo_SetAngle(13);
		Delay_ms(1000);
	}
	
	Servo_SetAngle(180);
	Delay_ms(1000);
	
}
