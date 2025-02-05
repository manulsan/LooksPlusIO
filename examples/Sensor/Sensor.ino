/*
 LooksPlusIO Example for sensors
    Tested with ESP32( Olimax ESP32-GATEWAY B/D)
    No copyrights
*/
#include <LooksPlusIO.h>
#include "def.h"
#include <esp_random.h>

#ifdef _NTP_USED_
// if NTP used, "data created time" is used, if not server side time is used
#include <NTPClient.h>
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 0, 60000); // 0 for UTC offset, 60000 for update interval
#endif

bool g_emitRequired = false;
unsigned long g_prevMillis = 0;
float g_fValues[NUM_OF_DATA_FIELDS] = {0, 0}; // data store buf
LooksPlusIO looksplusIO(DEVICE_SN, NUM_OF_DATA_FIELDS);

//--------------------------------------------------------------------
// name :setup
//--------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);

    initWIFI();
    initSensor();

#ifdef _NTP_USED_
    ntpClient.begin();
    ntpClient.update();
#endif

    looksplusIO.init(onCommandCallback, onConnectCallback);
    Serial.printf("----- ARDUINO SKETCH FOR SENSORS -----\n\r");
}

//--------------------------------------------------------------------
// name : loop
//--------------------------------------------------------------------
void loop()
{
    float fValue;
#ifdef _NTP_USED_
    ntpClient.update();
#endif
    unsigned long curMillis = millis();
    if (looksplusIO.isConnected() && (curMillis - g_prevMillis) >= UPLOAD_INTERVAL || g_emitRequired)
    {
        fValue = getTemperature();
        if (fValue != g_fValues[0])
        {
            g_fValues[0] = fValue;
            g_emitRequired = true;
        }

        fValue = getHumidity();
        if (fValue != g_fValues[1])
        {
            g_fValues[1] = fValue;
            g_emitRequired = true;
        }
#ifdef _NTP_USED_
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS, ntpClient.getEpochTime());
#else
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS);
#endif
        g_prevMillis = curMillis;
        g_emitRequired = false;
    }
    looksplusIO.loop();
}

//--------------------------------------------------------------------
// name : onCommandCallback
//--------------------------------------------------------------------
void onConnectCallback(bool status)
{
    g_prevMillis = millis();
    Serial.printf("onConnectCallback, status=%s\n\r", status ? "Connected" : "Disconnected");
    if (!status)
        Serial.printf("\tCheck if SN is registered on Server.  URL: %s\n\r",
                      looksplusIO.getUrlPath());
}

//--------------------------------------------------------------------
// name : onCommandCallback
//--------------------------------------------------------------------
void onCommandCallback(const char *payload, size_t length)
{
    Serial.printf("onCommandCallback, payload=%s\n\r", payload);
    DynamicJsonDocument doc(1024);
    DeserializationError err = deserializeJson(doc, String(payload));

    if (err)
        Serial.printf("err: deserializeJson() returned ", (const char *)err.f_str());
    else
    {
        auto cmd = doc["cmd"].as<const char *>();
        if (strcmp(cmd, "sync") == 0)
            g_emitRequired = true;
        else if (strcmp(cmd, "reboot") == 0)
            ESP.restart();
        else
            Serial.printf("cmd %s is not defined\n\r", cmd);
    }
}

//--------------------------------------------------------------------
// name: getTemperature
//--------------------------------------------------------------------
float getTemperature()
{
    // make your own sensor function here
    uint32_t data1 = esp_random() % 100;
    uint32_t data2 = esp_random() % 100;
    return (float)data1 + (float)(data2 / 100.0);
}

//--------------------------------------------------------------------
// name: getHumidity
//--------------------------------------------------------------------
float getHumidity()
{
    // make your own sensor function here
    uint32_t data1 = esp_random() % 100;
    uint32_t data2 = esp_random() % 100;
    return (float)data1 + (float)(data2 / 100.0);
}

//--------------------------------------------------------------------
// name : initSensor
//--------------------------------------------------------------------
void initSensor()
{
    // init your sensor here
}

//--------------------------------------------------------------------
// name : initWIFI
//--------------------------------------------------------------------
void initWIFI()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.printf(".");
    }
    Serial.printf("\n\rWiFi connected!!!\tIP address: ");
    Serial.println(WiFi.localIP());
}