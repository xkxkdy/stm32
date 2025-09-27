#include "stm32f10x.h"
#include "onenet.h"
#include "esp8266.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dht11.h"
#include "oled.h"
#include "led.h"
#include <string.h>

/*****************辰哥单片机设计******************
											STM32
 * 项目			:	OneNET云平台接入实验                     
 * 版本			: V1.0
 * 日期			: 2024.10.5
 * MCU			:	STM32F103C8T6
 * 接口			:	参看onenet.h							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥 

**********************BEGIN***********************/

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

u8 temperature=0,humidity=0,temperature_temp=0,humidity_temp=0;

int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
	Delay_Init();									//systick初始化
	Usart1_Init(115200);							//串口1，打印信息用
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	OLED_Init();
	LED_Init();
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
	 while(DHT11_Init())
	{
		delay_ms(200);
	}
	OLED_ShowString(20,24,"Networking",16,1);
	ESP8266_Init();					//初始化ESP8266
	OLED_ShowString(0,24,"                ",16,1);
	OLED_ShowString(0,24," Connected to",16,1);
	OLED_ShowString(30,48,"ONENET",16,1);
	UsartPrintf(USART_DEBUG, "Connect MQTTs Server...\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
	UsartPrintf(USART_DEBUG, "NET_OK\r\n");
	
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
		OLED_Clear();
		
		OLED_ShowChinese(0,0,0,16,1);  //温
		OLED_ShowChinese(18,0,1,16,1); //湿
		OLED_ShowChinese(36,0,2,16,1); //度
		OLED_ShowChinese(54,0,3,16,1); //检
		OLED_ShowChinese(72,0,4,16,1); //测
		OLED_ShowChinese(90,0,5,16,1); //装
		OLED_ShowChinese(108,0,6,16,1);//置
		OLED_ShowChinese(0,24,0,16,1);  //温
		OLED_ShowChinese(18,24,2,16,1); //度
		OLED_ShowChinese(0,48,1,16,1);  //湿
		OLED_ShowChinese(18,48,2,16,1); //度
		OLED_ShowString(80,24 ,"  C",16,1);
		OLED_ShowString(80,48 ,"  %",16,1);
	OLED_Refresh();
	OneNET_Subscribe();

	
	while(1)
	{
		
		if(++timeCount >= 100)									//发送间隔1s
		{
			DHT11_Read_Data(&temperature,&humidity,&temperature_temp,&humidity_temp);	//读取温湿度值		
			OLED_ShowNum(51,24,temperature,2,16,1);//显示温度
			OLED_ShowString(70,24,".",16,1);
			OLED_ShowNum(75,24,temperature_temp,2,16,1);
			
			OLED_ShowNum(51,48,humidity,2,16,1);	//显示湿度
			OLED_ShowString(70,48,".",16,1);
			OLED_ShowNum(75,48,humidity_temp,2,16,1);
			UsartPrintf(USART_DEBUG,"TEMP:%d HUMI:%d\r\n",temperature,humidity);
			//UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据订阅全部物模型相关主题
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		delay_ms(10);
	
	}

}
