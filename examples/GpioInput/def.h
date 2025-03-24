#ifndef _DEF_H_
#define _DEF_H_

#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

// #define _NTP_USED_

#define UPLOAD_INTERVAL 20000             // 20 seconds
#define DEVICE_SN "0001043C0024120000001" // replace with your device serial number
#define ON 1
#define OFF 0

// you can define the number of sensors here
#define NUM_OF_DATA_FIELDS 3 // number of sensors : min=1 , max=10

// OLIMAX  ESP32-Gateway B/D GPIOs,  HappyElec B/D I/O :19, 21, 22, 12
#define GPIO_PIN_1 34 // button 1
#define GPIO_PIN_2 21 // ESP-OLIMAX B/D, REL1 but used as input here for test
#define GPIO_PIN_3 22 //        ""

void initWifi();
void initGPIO();
bool readGPIOs();

void onDataCallback(const char *payload, size_t length);
void onConnectCallback(bool status);
#endif