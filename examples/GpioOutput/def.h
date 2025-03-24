#ifndef _DEF_H_
#define _DEF_H_

#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

// #define _NTP_USED_

#define ON 1
#define OFF 0
#define UPLOAD_INTERVAL 5000

#define DEVICE_SN "000303FF0024110000001" // replace with your device serial number

// you can define the number of sensors here
#define NUM_OF_DATA_FIELDS 3 // number of sensors : MAX 10

// OLIMAX  ESP32-Gateway B/D GPIOs HappyElec B/D I/O :19, 21, 22, 12
#define GPIO_PIN_1 32 // Relay 1
#define GPIO_PIN_2 33 // Relay 2
#define GPIO_PIN_3 34 // LED3 ?  but used as output here for test

void initWifi();
void initGPIO();
void setOutput(int f, float v);
void setOutputAll(float v);

void onDataCallback(const char *payload, size_t length);
void onConnectCallback(bool status);
#endif