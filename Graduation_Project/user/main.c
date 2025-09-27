#include "stm32f10x.h"
#include "onenet.h"
#include "esp8266.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "OLED.h"
#include "AD.h"
#include "Timer.h"
#include <string.h>

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"


/*
		功能		初始化硬件外设
		输入		无
		输出		无
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	Usart2_Init(115200);							//串口2，驱动ESP8266用
  key_Init();
	LED_Init();
	OLED_Init();
	Timer2_Init(2499,7199);
	Timer3_Init(4999,7199);
	AD_Init();	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
}



uint16_t Light_ADValue;
float Voltage;

extern uint8_t auto_manual_mode;
extern uint8_t key_val;

uint8_t Key_control_LED = 0;			//根据按键来判断是否开灯
uint8_t Key_control_MODE = 0;			//控制当前模式
uint8_t infrared_status = 0;
uint8_t su03t_status = 0;
uint8_t flag = 0;									//达到与开灯是一种对应关系，以至于不会再关灯后无法在开灯

/*
		功能		主函数
		输入		无
		输出		无
*/
int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266


	UsartPrintf(USART_DEBUG, "Connect MQTTs Server...\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		DelayXms(500);
	UsartPrintf(USART_DEBUG, "Connect MQTT Server Success\r\n");
	while(OneNet_DevLink())			//接入OneNET
		DelayXms(300);
	OneNET_Subscribe();		//初始化，订阅平台
	
	while(1)
	{
		su03t_status = Get_SU03T_val();
		infrared_status = Get_infrared_val();
		Light_ADValue = AD_GetValue();
		Voltage = (float)Light_ADValue / 4095 * 3.3;
		//上发数据到平台
		if(++timeCount >= 500)									//发送间隔5s
		{
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			timeCount = 0;
			ESP8266_Clear();
		}		
		//接收云端的消息
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}
		
		//总是检测按键的变化
		//1--开灯，2--关灯
		Key_control_LED = Get_Key1_Value();	
		Key_control_MODE = 	Get_Key2_Value();
		if(Key_control_LED == 2)
		{
			if(auto_manual_mode == 3)	//自动模式
			{
				;
			}
			else if(auto_manual_mode == 2 && flag == 1)
			{
				flag = 0;
				key_val = 0;
			}
		}
		if(Key_control_LED == 1)		//表示要开灯
		{
			flag = 1;
			key_val = 1;
			auto_manual_mode = 2;		//切换到手动模式
		}
		//低电平--3，高电平--4
		//控制模式的按键只想要切换为自动模式就行
		//两个按键同时按下，仍然是自动模式
		if(Key_control_MODE == 3)
		{
			auto_manual_mode = 3;
		}
	
		
		DelayMs(10);	
	}

}
