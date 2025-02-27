#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>  // 如果你使用的是 ESP32，请改为 #include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>

SoftwareSerial mySerial(14,12); // RX=d5,TX=d6
String UART_String = "";

// Wi-Fi 配置信息
const char* ssid = "test";        // 你的 Wi-Fi 名称
const char* password = "13630025597"; // 你的 Wi-Fi 密码


void setup() {
  Serial.begin(9600);    // 内部串口初始化
  mySerial.begin(9600);
  mySerial.listen();

  // 连接到 Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);  // 启动 Wi-Fi 连接

  // 等待 Wi-Fi 连接成功
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // 打印 ESP 的 IP 地址



}


void loop() {
  if (mySerial.available()) {
    UART_String += (char) mySerial.read();  // 读取 STM32 传来的数据
    Serial.print("stm32 data: ");
    Serial.println(UART_String);   // 打印数据
  }

}
