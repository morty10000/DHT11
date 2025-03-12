#include "stm32f10x.h"
#include "Serial.h"
#include "delay.h"
#include "OLED.h"
#include "DHT11.h"
#include <stdio.h>  // ȷ�� snprintf �����ɼ�
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

char atDataSend[200]; // ����Ԥ���㹻�ռ䣨����ʵ�����������


void SendDataToESP8266(void)
{
    char dataStr[50];  // ȷ���ַ��������㹻���ܹ��洢ת���������
    
    // ʹ�ø�������ʽ��������һλС��
     temp = rec_data[2] + rec_data[3] / 10.0f;  // �¶ȣ����� 14.7��
     humi = rec_data[0] + rec_data[1] / 10.0f;  // ʪ�ȣ����� 90.0��
    
    // ��ʽ������Ϊ "14.7 90.0" ��ʽ
    int result = sprintf(dataStr, "%.1f %.1f", temp, humi);  // %.1f ��֤����1λС��
   // OLED_ShowNum(4,9,humi,4);
	//OLED_ShowString(3,1,dataStr);
}




void GenerateATCommand(void) {
    // �����㹻��Ļ����������� 200 �ֽڣ�

    const size_t buf_size = sizeof(atDataSend);

    // ���� AT ָ��
    int len = snprintf(atDataSend, buf_size,
        "AT+MQTTPUB=0,"
        "\"/sys/k1ou2S5PFlX/stm32/thing/event/property/post\","
        "\"{\\\"params\\\":{\\\"temp\\\":%.1f\\,\\\"humi\\\":%.1f}\\,\\\"version\\\":\\\"1.0.0\\\"}\","
        "1,0\r\n",
        temp, humi);

    // ��鳤���Ƿ����
    if (len < 0 || (size_t)len >= buf_size) {
        // ��������绺�������㣩
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
    Serial_SendString("AT+CWMODE=1\r\n");            // ����ΪSTAģʽ
    Delay_ms(500);                                   // �ȴ�ESP8266��Ӧ
    Serial_SendString("AT+CWJAP=\"test\",\"13630025597\"\r\n");  // ����Wi-Fi
    Delay_ms(2000);                                   // �ȴ�Wi-Fi����
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
	Serial_SendString(atAli);  // ���Ӱ�����MQTT Broker
    Delay_ms(1000);;  // �ȴ�����
}

void SendDataToAliyun(void)
{
    // ����ͨ��MQTTЭ�鷢��
    // ʹ�ð����Ƶ��������豸��Ϣ��������
	GenerateATCommand();
    Serial_SendString(atDataSet);  // ������Ϣ
    Delay_ms(1000);
   // Serial_SendString(atDataTest);  // ��������
    Delay_ms(1000);

	//OLED_ShowNum(4,1,humi,4);
	Serial_SendString(atDataSend);  // ��������
}

//���İ���������
void Subscribe_Aliyun_Topic(void) {
    Serial_SendString(subscribe_cmd);
    Delay_ms(500);
}
