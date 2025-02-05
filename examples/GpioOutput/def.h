#ifndef _DEF_H_
#define _DEF_H_

#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

#define ON 1
#define OFF 0
#define UPLOAD_INTERVAL 5000
// #define DEVICE_SN "0001013C0024120000001" // replace with your device serial number
#define DEVICE_SN "000303FF0024110000001" // replace with your device serial number

// #define _NTP_USED_

#define NUM_OF_DATA_FIELDS 3 // number of sensors : MAX 10
//------------------------------------------------------------------------
// HappyElec B/D Outpus :     19, 21, 22, 12

//------------------------------------------------------------------------
// OLIMAX Gateway B/D GPIOs
#define GPIO_PIN_1 32 // REL1
#define GPIO_PIN_2 33 // REL2
#define GPIO_PIN_3 34 // LED3 ?  but used as output here for test

void initWifi();
void initGPIO();
void setOutput(int f, float v);
void setOutputAll(float v);

void onCommandCallback(const char *payload, size_t length); // callback func that is received from apps command
#endif