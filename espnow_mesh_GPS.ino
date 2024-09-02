#include <Arduino.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <WifiEspNowBroadcast.h>

// The serial port for GPS data, using ESP32S3 manual pin mapping
#define UART_RX_PIN 18 // GPIO18 for RX
#define UART_TX_PIN 17 // GPIO17 for TX

TinyGPSPlus gps; // Create a TinyGPSPlus object

unsigned long lastSendTime = 0; // 用于控制发送频率
const unsigned long sendInterval = 100; // 发送间隔（毫秒）

void sendGPSData();
void processRx(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t* buf, size_t count, void* arg);
//void printGPSData();

void setup() {
    Serial.begin(38400); // Connects to the computer, baud rate is customizable
    Serial2.begin(38400, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN); // Connects to GPS device

    WiFi.persistent(false);
    bool ok = WifiEspNowBroadcast.begin("ESPNOW", 3);
    if (!ok) {
        Serial.println("WifiEspNowBroadcast.begin() failed");
        ESP.restart();
    }

    WifiEspNowBroadcast.onReceive(processRx, nullptr);
    Serial.print("MAC address of this node is ");
    Serial.println(WiFi.softAPmacAddress());
    Serial.println("Device started, receiving GPS data...");
}

void loop() {
    while (Serial2.available() > 0) {
        char c = Serial2.read(); // Read a character from the GPS device
        gps.encode(c); // Feed the GPS character into TinyGPS++ decoder
    }

    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval) {
        sendGPSData();
        lastSendTime = currentTime;
    }

    WifiEspNowBroadcast.loop();
}

void sendGPSData() {
    char msg[256];
    int len = 0;

    // 直接添加发送者的MAC地址，不使用 "Message from"
    String macAddress = WiFi.softAPmacAddress();
    len += snprintf(msg + len, sizeof(msg) - len, "%s\n", macAddress.c_str());

    // 检查时间是否有效并直接输出
    if (gps.time.isValid()) {
        len += snprintf(msg + len, sizeof(msg) - len, "Time: %02d:%02d:%02d\n", gps.time.hour() + 8, gps.time.minute(), gps.time.second());
    }

    // 检查位置数据是否有效并直接输出
    if (gps.location.isValid()) {
        len += snprintf(msg + len, sizeof(msg) - len, "Lat: %.6f, Lon: %.6f\n", gps.location.lat(), gps.location.lng());
    }
    // 通过ESP-NOW发送GPS数据
    WifiEspNowBroadcast.send(reinterpret_cast<const uint8_t*>(msg), len);

    // 在本地串行端口打印发送的消息
    Serial.print(msg);
}



void processRx(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t* buf, size_t count, void* arg) {
    // 打印接收到的ESP-NOW消息
    for (size_t i = 0; i < count; ++i) {
        Serial.print((char)buf[i]);
    }
    Serial.println(); // 确保在消息结束后换行
}

