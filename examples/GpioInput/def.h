#ifndef _DEF_H_
#define _DEF_H_

#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

#define ON 1
#define OFF 0
#define UPLOAD_INTERVAL 10000
// #define DEVICE_SN "0001013C0024120000001" // replace with your device serial number
#define DEVICE_SN "000101160024050000009" // replace with your device serial number

// #define _NTP_USED_

#define NUM_OF_DATA_FIELDS 3 // number of sensors : MAX 10
//------------------------------------------------------------------------
// HappyElec B/D Outputs :     19, 21, 22, 12

//------------------------------------------------------------------------
// OLIMAX Gateway B/D GPIOs
#define GPIO_PIN_1 34 // button 1
#define GPIO_PIN_2 21 // ESP-OLIMAX B/D, REL1 but used as input here for test
#define GPIO_PIN_3 22 //        ""

void initWifi();
void initGPIO();
bool readGPIOs();
void onCommandCallback(const char *payload, size_t length); // callback func that is received from apps command
#endif