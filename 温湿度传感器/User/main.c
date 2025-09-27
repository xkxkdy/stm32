#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"

u8 temp;
u8 humi;
u8 temp_point;
u8 humi_point;


int main(void)
{
	
	OLED_Init();
	DHT11_Init();
	
	//显示“温度：”
	OLED_ShowChinese(1,1, 0);
	OLED_ShowChinese(1,2, 1);
	OLED_ShowChar(1, 5, ':');
	OLED_ShowChar(1, 8, '.');		//小数点
	OLED_ShowChar(1, 11, 'C');	
	//显示“湿度：”
	OLED_ShowChinese(2,1, 2);
	OLED_ShowChinese(2,2, 1);
	OLED_ShowChar(2, 5, ':');	
	OLED_ShowChar(2, 8, '.');		//小数点
	OLED_ShowChar(2, 11, '%');
	
	while (1)
	{
		DHT11_Read_Data(&temp,&humi,&temp_point,&humi_point);
		Delay_ms(1000);
		//显示温度数据
		OLED_ShowNum(1,6,temp,2);
		OLED_ShowNum(1,9,temp_point,2);
		//显示湿度数据
		OLED_ShowNum(2,6,humi,2);
		OLED_ShowNum(2,9,humi_point,2);
	}

	
}
