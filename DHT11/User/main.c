#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "Serial.h"
#include "ESP8266.h"
#include <string.h>  // 添加此行以声明strstr和strlen
#include <ctype.h>  // 添加此行以声明isspace和isdigit
#include "LED.h"
extern unsigned int rec_data[4];
#define RX_BUFFER_SIZE 512 // JSON数据可能较长，缓冲区需足够大
char rx_buffer[RX_BUFFER_SIZE];//json数组
volatile uint16_t rx_index = 0;
volatile uint8_t json_received = 0;  // 新增：JSON接收完成标志
void SendDataToESP8266(void);
extern float humi;
extern float temp;
// 在串口中断服务函数中接收数据
char mqttMsg;
uint8_t msgIndex = 0;
int parse_led_manually(char *json_str);
void ParseLEDCommand(char *json);
int main()
{
	int value = 3;
	OLED_Init();
	Serial_Init();
	LED_Init();
	OLED_ShowString(1, 1, "temp:");	
	OLED_ShowString(1, 9, ".");	
	OLED_ShowString(1, 12, "C");	
	OLED_ShowString(2, 1, "humi:"); 						
	OLED_ShowString(2, 9, ".");
	OLED_ShowString(2, 12, "%");	
	
	ConnectWiFi();
 	ConnectToMQTT();
	ConnectToAliyun();
	Subscribe_Aliyun_Topic();
	
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
		
		
//		value=parse_led_manually(rx_buffer);
//		OLED_ShowNum(4,1,value,1);
		 if (json_received){ // ✅ 仅处理新数据
			value = parse_led_manually(rx_buffer);
			OLED_ShowNum(4,1,value,1);
			json_received = 0; // 重置标志
			rx_index = 0;      // 清空缓冲区索引
		}
		if(value==1)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_0);                   // 高电平
		}
		if(value==0)
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_0);                   // 高电平
		}
	}
}

void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t data = USART_ReceiveData(USART1);

        // 存储数据（防溢出）
        if (rx_index < RX_BUFFER_SIZE - 1) {
            rx_buffer[rx_index++] = data;
        } else {
            // 溢出时丢弃旧数据（或自定义处理）
            rx_index = 0;
        }

        // 检测JSON结束符'}'并添加终止符
        if (data == '}') {
            rx_buffer[rx_index] = '\0'; // 正确终止字符串
            json_received = 1;         // 标记接收完成
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }

    // 处理溢出错误
    if (USART_GetITStatus(USART1, USART_IT_ORE) != RESET) {
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
        USART_ReceiveData(USART1);
    }
}
// 手动解析JSON并提取led值（成功返回led值，失败返回-1）
int parse_led_manually(char *json_str) {
    // 关键路径：items -> led -> value
    char *items_key = "\"items\":";
    char *led_key = "\"led\":";
    char *value_key = "\"value\":";
    
    // 1. 查找 "items": { ... }
    char *ptr = strstr(json_str, items_key);
    if (!ptr) return -1;
    ptr += strlen(items_key);
    
    // 跳过空格和冒号，直到找到 '{'
    while (*ptr && (isspace(*ptr) || *ptr == ':')) ptr++;
    if (*ptr != '{') return -1;
    ptr++; // 跳过 '{'

    // 2. 在 items 内查找 "led": { ... }
    ptr = strstr(ptr, led_key);
    if (!ptr) return -1;
    ptr += strlen(led_key);
    
    // 跳过空格和冒号，直到找到 '{'
    while (*ptr && (isspace(*ptr) || *ptr == ':')) ptr++;
    if (*ptr != '{') return -1;
    ptr++; // 跳过 '{'

    // 3. 在 led 对象内查找 "value":
    ptr = strstr(ptr, value_key);
    if (!ptr) return -1;
    ptr += strlen(value_key);
    
    // 跳过空格和冒号
    while (*ptr && (isspace(*ptr) || *ptr == ':')) ptr++;

    // 4. 提取数值
    int value = 0;
    while (isdigit(*ptr)) {
        value = value * 10 + (*ptr - '0');
        ptr++;
    }

    return value;
}
