
// LooksPlusIO sensor example
// - 1 Verified  with ESP32( Olimax ESP32-GATEWAY B/D)
// - 2. Verified with ESP8266( NodeMCU 1.0 (ESP-12E Module)

#include <LooksPlusIO.h>
#include "def.h"

// if NTP used, "data created time" is used, if not server side time is used
#ifdef _NTP_USED_
#include <NTPClient.h>
WiFiUDP ntpUDP;

// 0 for UTC offset, 60000 for update interval
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 0, 60000);
#endif

bool g_uploadRequired = true;
unsigned long g_prevMillis = 0;
float g_fValues[NUM_OF_DATA_FIELDS] = {0, 0}; // data store buf
LooksPlusIO looksplusIO(DEVICE_SN, NUM_OF_DATA_FIELDS);

//-----------------------------------------------------------------------------
// name :setup
//-----------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);

    initWIFI();
    initSensor();
#ifdef _NTP_USED_
    ntpClient.begin();
    ntpClient.update();
#endif

    looksplusIO.init(onDataCallback, onConnectCallback, true);
    Serial.printf("\n\r----- atCloud365.com  Sensor Example -----\n\r");
}

//-----------------------------------------------------------------------------
// name : loop
//-----------------------------------------------------------------------------
void loop()
{
    float fValue;
#ifdef _NTP_USED_
    ntpClient.update();
#endif
    unsigned long curMillis = millis();
    if (looksplusIO.isConnected() && (curMillis - g_prevMillis) >= UPLOAD_INTERVAL || g_uploadRequired)
    {
        g_fValues[0] = getTemperature();
        g_fValues[1] = getHumidity();

#ifdef _NTP_USED_
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS, ntpClient.getEpochTime());
#else
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS);
#endif
        g_prevMillis = curMillis;
        g_uploadRequired = false; // reset, g_uploadRequired is used at other examples
    }
    looksplusIO.loop();
}

//-----------------------------------------------------------------------------
// name : onConnectCallback
//-----------------------------------------------------------------------------
void onConnectCallback(bool status)
{
    try
    {
        Serial.printf("onConnectCallback, status=%s\n\r", status ? "Connected" : "Disconnected");
        if (!status)
            Serial.printf("\tCheck if device-sn is registered on atCloud365 as my device.  URL: %s\n\r",
                          looksplusIO.getUrlPath());
    }
    catch (const std::exception &e)
    {
        Serial.printf("err: %s ", e.what());
    }
}

//-----------------------------------------------------------------------------
// name : onDataCallback
//-----------------------------------------------------------------------------
void onDataCallback(const char *payload, size_t length)
{
    try
    {
        Serial.printf("onDataCallback, payload=%s\n\r", payload);
        DynamicJsonDocument doc(1024);
        DeserializationError err = deserializeJson(doc, String(payload));

        if (err)
            Serial.printf("err: deserializeJson() returned ", (const char *)err.f_str());
        else
        {
            if (doc.containsKey("cmd"))
            {
                auto cmd = doc["cmd"].as<const char *>();
                if (strcmp(cmd, "sync") == 0)
                    g_uploadRequired = true;
                else if (strcmp(cmd, "reboot") == 0)
                    ESP.restart();
                else
                    Serial.printf("cmd %s is not defined\n\r", cmd);
            }
            else
            {
                Serial.printf("object does not have cmd prop\n\r");
            }
        }
    }
    catch (const std::exception &e)
    {
        // std::cerr << e.what() << '\n';
        Serial.printf("err: %s ", e.what());
    }
}

//-----------------------------------------------------------------------------
// name: getTemperature
//-----------------------------------------------------------------------------
float getTemperature()
{
#if defined(ESP32)
    // make your own sensor function here
    uint32_t data1 = esp_random() % 100;
    uint32_t data2 = esp_random() % 100;
#else
    uint32_t data1 = 20;
    uint32_t data2 = 80;
#endif
    return (float)data1 + (float)(data2 / 100.0);
}

//-----------------------------------------------------------------------------
// name: getHumidity
//-----------------------------------------------------------------------------
float getHumidity()
{
// make your own sensor function here
#if defined(ESP32)
    // make your own sensor function here
    uint32_t data1 = esp_random() % 100;
    uint32_t data2 = esp_random() % 100;
#else
    uint32_t data1 = 26;
    uint32_t data2 = 88;
#endif
    return (float)data1 + (float)(data2 / 100.0);
}

//-----------------------------------------------------------------------------
// name : initSensor
//-----------------------------------------------------------------------------
void initSensor()
{
    // init your sensor here
}

//-----------------------------------------------------------------------------
// name : initWIFI
//-----------------------------------------------------------------------------
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