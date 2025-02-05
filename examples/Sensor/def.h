#ifndef _DEF_H_
#define _DEF_H_

#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

// #define _NTP_USED_

#define UPLOAD_INTERVAL 5000
#define DEVICE_SN "0001040C002406000000A" // replace with your device serial number

#define NUM_OF_DATA_FIELDS 2 // number of sensors : MAX 10
void initWifi();
void initSensor();
void onCommandCallback(const char *payload, size_t length); // callback func that is received from apps command
#endif