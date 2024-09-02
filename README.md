Remember to add the following two libraries:

https://github.com/yoursunny/WifiEspNow

https://github.com/mikalhart/TinyGPSPlus

No need to add MAC,But remember to change the GPS pins，the required change codes are as follows：

#define UART_RX_PIN 18 // GPIO18 for RX

#define UART_TX_PIN 17 // GPIO17 for TX

Serial2.begin(38400, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

Since I have ESP32-S3 in my hand and iomux features, if you have ESP32 in hand and do not have iomux, then you need to change the above code.

Delete

#define UART_RX_PIN 18 // GPIO18 for RX

#define UART_TX_PIN 17 // GPIO17 for TX

change Serial2.begin(38400, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN); 

become Serial2.begin(38400);




