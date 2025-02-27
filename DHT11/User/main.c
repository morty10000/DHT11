#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "Serial.h"
#include "ESP8266.h"
extern unsigned int rec_data[4];

void SendDataToESP8266(void);
extern float humi;
extern float temp;
int main()
{

	OLED_Init();
	Serial_Init();
	
	OLED_ShowString(1, 1, "temp:");	
	OLED_ShowString(1, 9, ".");	
	OLED_ShowString(1, 12, "C");	
	OLED_ShowString(2, 1, "humi:"); 						
	OLED_ShowString(2, 9, ".");
	OLED_ShowString(2, 12, "%");	
	
	ConnectWiFi();
 	ConnectToMQTT();
	ConnectToAliyun();
	
	
	while(1)
	{	
		
		Delay_s(1);
		DHT11_REC_Data(); //接收温度和湿度的数据
	
	    OLED_ShowNum(1,7,rec_data[2],2);
		OLED_ShowNum(1,10,rec_data[3],2);
		OLED_ShowNum(2,7,rec_data[0],2);
		OLED_ShowNum(2,10,rec_data[1],2);
		SendDataToAliyun();
		SendDataToESP8266();
		
		
	}
}
