#ifndef _DEF_H_
#define _DEF_H_

#if defined(ESP32)
#include <esp_random.h>
#endif

#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

// #define _NTP_USED_
#define UPLOAD_INTERVAL 20000             // 20 seconds
#define DEVICE_SN "0001043C0024120000001" // replace with your device serial number

// you can define the number of sensors here
#define NUM_OF_DATA_FIELDS 3 // number of sensors : min=1 , max=10
void initWifi();
void initSensor();

void onDataCallback(const char *payload, size_t length);
void onConnectCallback(bool status);
#endif