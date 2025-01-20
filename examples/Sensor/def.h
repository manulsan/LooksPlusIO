#ifndef _DEF_H_
#define _DEF_H_

#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

// #define _NTP_USED_

#define UPLOAD_INTERVAL 5000
// #define DEVICE_SN "000303FF0024110000001" // replace with your device serial number
#define DEVICE_SN "0001013C0024120000001" // replace with your device serial number
#define NUM_OF_DATA_FIELDS 2              // number of sensors : MAX 10
void initWifi();
void initSensor();
void commandCB(const char *payload, size_t length); // callback func that is received from apps command
#endif