#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "DHT11.h"


/**
  * @brief  复位DHT11
  * @param  无
  * @retval 无
  */
void DHT11_Reset(void)
{
	DHT11_Mode_Change(OUT);
	DHT11_LOW;			//拉低信号
	Delay_ms(20);		//拉低20ms
	DHT11_HIGH;
	Delay_us(13);		//拉高13us
}

/**
  * @brief  等待DHT11回应
  * @param  无
  * @retval 1：不存在			0：存在
  */
u8 DHT11_Check(void)
{
	u8 reply = 0;
	DHT11_Mode_Change(IN);
	//数据手册说明初始时DHT11会先拉低40-80us		之后会再次拉高40-80us	
	while(GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN) && reply < 100 )
	{
		reply++;
		Delay_us(1);
	}
	
	if(reply >= 100) return 1;
	else						 reply = 0;
	while(!GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN) && reply < 100)
	{
		reply++;
		Delay_us(1);
	}
	if(reply >= 100) return 1;
	
	return 0;
}

/**
  * @brief  读取DHT11一个位
  * @param  无
  * @retval 0 和 1
  */
u8 DHT11_Read_Bit(void)
{
	u8 reply = 0;
	//
	while(GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN) && reply < 100 )
	{
		reply++;
		Delay_us(1);
	}
	
	reply = 0;
	while(!GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN) && reply < 100)
	{
		reply++;
		Delay_us(1);
	}
	Delay_us(40);
	if(GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN))	return 1;
	return 0;
}

/**
  * @brief  读取DHT11一个字节
  * @param  无
  * @retval 将八位字节放在一个字中
  */
u8 DHT11_Read_Byte(void)
{
	u8 i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 		//向左移位1位
		dat|= DHT11_Read_Bit();	//相当于与0x00 或 0x01 按位或
	}						    
	return dat;
}

/**
  * @brief  将读取到的数据放到指定位置
  * @param  temp：温度整数 humi：湿度整数 temp_point：温度小数 humi_point：湿度小数
	* @retval 0:正常	1：读取失败
  */
u8 DHT11_Read_Data(u8 *temp,u8 *humi,u8 *temp_point,u8 *humi_point)
{        
 	u8 buf[5];
	u8 i;
	DHT11_Reset();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		//buf[4]是校验位，前四个数据相加等于buf[4]说明数据发送是没错的
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
			*temp_point = buf[1];
			*humi_point = buf[3];
		}
	}
	else return 1;
	return 0;	    
}



/**
  * @brief  初始化DHT11引脚，同时检测DHT11是否存在
  * @param  无
  * @retval 1：不存在			0：存在
  */
u8 DHT11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
	
	DHT11_HIGH;
	
	return DHT11_Check();
}

/**
  * @brief  DHT11引脚模式改变
  * @param  无
  * @retval 无
  */
void DHT11_Mode_Change(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructre;
	
	if(mode)
	{
		GPIO_InitStructre.GPIO_Pin = DHT11_PIN;
		GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructre.GPIO_Mode = GPIO_Mode_Out_PP;
	}
	else
	{
		GPIO_InitStructre.GPIO_Pin = DHT11_PIN;
		GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructre.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	}
	
	GPIO_Init(DHT11_PORT,&GPIO_InitStructre);
}

