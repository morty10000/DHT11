#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>


SoftwareSerial mySerial(14, 12);  // RX=d5, TX=d6
String UART_String = "";
float temp ;
float humi ;
// Wi-Fi 配置信息
const char* ssid = "test";        // 你的 Wi-Fi 名称
const char* password = "13630025597"; // 你的 Wi-Fi 密码

// MQTT 配置
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "iot-06z00huu9u3ft33.mqtt.iothub.aliyuncs.com";
int port = 1883;

const char inTopic[] = "/sys/k1ou2S5PFlX/esp8266/thing/service/property/set";
const char outTopic[] = "/sys/k1ou2S5PFlX/esp8266/thing/event/property/post";

const long interval = 10000;
unsigned long previousMillis = 0;

int count = 0;
String inputString = "";

void setup() {
  // 初始化串口
  Serial.begin(9600);
  while (!Serial) {
    ; // 等待串口连接
  }

  // 初始化软串口
  mySerial.begin(9600);
  mySerial.listen();

  // 初始化 Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);  // 启动 Wi-Fi 连接
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // 打印 ESP 的 IP 地址

  // MQTT 配置
  mqttClient.setId("k1ou2S5PFlX.esp8266|securemode=2,signmethod=hmacsha256,timestamp=1739693379559|");
  mqttClient.setUsernamePassword("esp8266&k1ou2S5PFlX", "650fd5fe8e9adfc972b89186b0ba60105d05d1f9d1deb4124432eb6764b89f3c");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  Serial.println("You're connected to the MQTT broker!");

  // 设置消息接收回调
  mqttClient.onMessage(onMqttMessage);

  // 订阅主题
  Serial.print("Subscribing to topic: ");
  Serial.println(inTopic);
  mqttClient.subscribe(inTopic, 1);  // 设置 QoS 为 1
  Serial.print("Waiting for messages on topic: ");
  Serial.println(inTopic);
}

void loop() {
  mqttClient.poll();

  // 读取 UART 数据
  if (mySerial.available()) {
    UART_String += (char)mySerial.read();
    Serial.print("STM32 data: ");
    Serial.println(UART_String);  // 打印数据

    int spaceIndex = UART_String.indexOf(' ');  // 查找空格的位置
    if (spaceIndex != -1) {
      String tempStr = UART_String.substring(0, spaceIndex);  // 获取温度部分
      String humiStr = UART_String.substring(spaceIndex + 1); // 获取湿度部分

      // 转换为浮动数值
      temp = tempStr.toFloat();
      humi = humiStr.toFloat();
    }
  }
  
  // 定时发送数据
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    String payload;
    DynamicJsonDocument json_msg(512);
    DynamicJsonDocument json_data(512);

    json_data["temp"] = temp;
    json_data["humi"] = humi;

    json_msg["params"] = json_data;
    json_msg["version"] = "1.0.0";

    serializeJson(json_msg, payload);

    Serial.print("Sending message to topic: ");
    Serial.println(outTopic);
    Serial.println(payload);

    bool retained = false;
    int qos = 1;
    bool dup = false;

    mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
    mqttClient.print(payload);
    mqttClient.endMessage();

    Serial.println();

    count++;
  }
}

void onMqttMessage(int messageSize) {
  // 接收到消息后，打印主题和内容
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', duplicate = ");
  Serial.print(mqttClient.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(mqttClient.messageQoS());
  Serial.print(", retained = ");
  Serial.print(mqttClient.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // 处理消息内容
  while (mqttClient.available()) {
    char inChar = (char)mqttClient.read();
    inputString += inChar;
    if (inputString.length() == messageSize) {
      DynamicJsonDocument json_msg(1024);
      DynamicJsonDocument json_item(1024);
      DynamicJsonDocument json_value(1024);

      deserializeJson(json_msg, inputString);
      String items = json_msg["items"];
      deserializeJson(json_item, items);
      String led = json_item["led"];
      deserializeJson(json_value, led);

      bool value = json_value["value"];
      if (value == 0) {
        // 关 LED
        Serial.println("off");
        digitalWrite(4, HIGH);  // LED 关闭
      } else {
        // 开 LED
        Serial.println("on");
        digitalWrite(4, LOW);   // LED 打开
      }
      inputString = "";
    }
  }
  Serial.println();
}
