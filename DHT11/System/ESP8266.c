#include "stm32f10x.h"
#include "Serial.h"
#include "delay.h"
#include "OLED.h"
#include "DHT11.h"
#include <stdio.h>  // 确保 snprintf 声明可见
#define WIFI_SSID        "test"
#define WIFI_PASSWD      "13630025597"

extern int rec_data[4];
float humi;
float temp;
char at[]="AT";
char atMQTT[] = "AT+MQTTUSERCFG=0,1,\"k1ou2S5PFlX.stm32|securemode=2\\,signmethod=hmacsha256\\,timestamp=1739697434391|\",\"stm32&k1ou2S5PFlX\",\"ee52781e013cb6542be876279981400eeeb0c250cdc32a733eb685af40c1d80f\",0,0,\"\"\r\n";
char subscribe_cmd[]  = "AT+MQTTSUB=0,\"/sys/k1ou2S5PFlX/html/thing/service/property/set\",1";
//AT+MQTTCONN=0,"<YourProductKey>.iot-as-mqtt.<Region>.aliyuncs.com",1883,1
char atAli[] = "AT+MQTTCONN=0,\"k1ou2S5PFlX.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,0\r\n";
char atDataSet[]="AT+MQTTSUB=0,\"/sys/k1ou2S5PFlX/stm32/thing/service/property/set\",0\r\n";
char atDataTest[] = "AT+MQTTPUB=0,\"/sys/k1ou2S5PFlX/stm32/thing/event/property/post\",\"{\\\"params\\\":{\\\"temp\\\":11\\,\\\"humi\\\":100}\\,\\\"version\\\":\\\"1.0.0\\\"}\",1,0\r\n";

char atDataSend[200]; // 建议预留足够空间（根据实际需求调整）


void SendDataToESP8266(void)
{
    char dataStr[50];  // 确保字符串数组足够大，能够存储转换后的数据
    
    // 使用浮点数格式化，保留一位小数
     temp = rec_data[2] + rec_data[3] / 10.0f;  // 温度（例如 14.7）
     humi = rec_data[0] + rec_data[1] / 10.0f;  // 湿度（例如 90.0）
    
    // 格式化数据为 "14.7 90.0" 形式
    int result = sprintf(dataStr, "%.1f %.1f", temp, humi);  // %.1f 保证保留1位小数
   // OLED_ShowNum(4,9,humi,4);
	//OLED_ShowString(3,1,dataStr);
}




void GenerateATCommand(void) {
    // 定义足够大的缓冲区（例如 200 字节）

    const size_t buf_size = sizeof(atDataSend);

    // 生成 AT 指令
    int len = snprintf(atDataSend, buf_size,
        "AT+MQTTPUB=0,"
        "\"/sys/k1ou2S5PFlX/stm32/thing/event/property/post\","
        "\"{\\\"params\\\":{\\\"temp\\\":%.1f\\,\\\"humi\\\":%.1f}\\,\\\"version\\\":\\\"1.0.0\\\"}\","
        "1,0\r\n",
        temp, humi);

    // 检查长度是否溢出
    if (len < 0 || (size_t)len >= buf_size) {
        // 处理错误（如缓冲区不足）
    }
	//OLED_ShowString(4,1,atDataSend);
}




void ConnectESP8266(void)
{
    Serial_SendString(at);           
    Delay_ms(500);                          
}

void ConnectWiFi(void)
{
    Serial_SendString("AT+CWMODE=1\r\n");            // 设置为STA模式
    Delay_ms(500);                                   // 等待ESP8266响应
    Serial_SendString("AT+CWJAP=\"test\",\"13630025597\"\r\n");  // 连接Wi-Fi
    Delay_ms(2000);                                   // 等待Wi-Fi连接
    OLED_ShowString(3,1,"WiFi");
}

void ConnectToMQTT(void)
{
    
    Serial_SendString(atMQTT);  
    Delay_ms(1000);  
}

void ConnectToAliyun(void)
{
    
    OLED_ShowString(3,6," MQTT...");
	Serial_SendString(atAli);  // 连接阿里云MQTT Broker
    Delay_ms(1000);;  // 等待连接
}

void SendDataToAliyun(void)
{
    // 假设通过MQTT协议发送
    // 使用阿里云的主题与设备信息发布数据
	GenerateATCommand();
    Serial_SendString(atDataSet);  // 订阅消息
    Delay_ms(1000);
   // Serial_SendString(atDataTest);  // 发布数据
    Delay_ms(1000);

	//OLED_ShowNum(4,1,humi,4);
	Serial_SendString(atDataSend);  // 发布数据
}

//订阅阿里云主题
void Subscribe_Aliyun_Topic(void) {
    Serial_SendString(subscribe_cmd);
    Delay_ms(500);
}
