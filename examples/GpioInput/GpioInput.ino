/*
 MIT License

Copyright (c) [2029] [Copyright Links]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
 */

#include <LooksPlusIO.h>
#include "def.h"

#ifdef _NTP_USED_
#include <NTPClient.h>
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 0, 60000); // 0 for UTC offset, 60000 for update interval
#endif

bool g_uploadRequired = false;
unsigned long g_prevMillis = 0;
float g_fValues[NUM_OF_DATA_FIELDS] = {0, 0, 0}; // data store buf
LooksPlusIO looksplusIO(DEVICE_SN, NUM_OF_DATA_FIELDS);

uint8_t g_gpio_pins[NUM_OF_DATA_FIELDS] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3}; // gpio pins

//----------------------------------------------------------------------------------
// name :setup
//----------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);

    initWIFI();
    initGPIO();

#ifdef _NTP_USED_
    ntpClient.begin();
    ntpClient.update();
#endif

    looksplusIO.init(onDataCallback, onConnectCallback, true); // true for debug, if no debug then pass false
    Serial.printf("\n\r----- atCloud365.com  GPIO INPUT Example -----\n\r");
}

//----------------------------------------------------------------------------------
// name : loop
//----------------------------------------------------------------------------------
void loop()
{
#ifdef _NTP_USED_
    ntpClient.update();
#endif
    bool bUpdated = readGPIOs();
    unsigned long curMillis = millis();
    if (looksplusIO.isConnected() && (curMillis - g_prevMillis) >= UPLOAD_INTERVAL || bUpdated || g_uploadRequired)
    {
#ifdef _NTP_USED_
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS, ntpClient.getEpochTime());
#else
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS);
#endif
        g_uploadRequired = false;
        g_prevMillis = curMillis;
    }
    looksplusIO.loop();
}

//----------------------------------------------------------------------------------
// name : onConnectCallback
//----------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------
// name : onDataCallback
//----------------------------------------------------------------------------------
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
        Serial.printf("err: %s ", e.what()); // std::cerr << e.what() << '\n';
    }
}

//----------------------------------------------------------------------------------
// name : initGPIO
//----------------------------------------------------------------------------------
void initGPIO()
{
    try
    {
        for (uint8_t i = 0; i < NUM_OF_DATA_FIELDS; i++)
        {
            pinMode(g_gpio_pins[i], INPUT_PULLUP);
            g_fValues[i] = (float)digitalRead(g_gpio_pins[i]);
        }
    }
    catch (const std::exception &e)
    {
        Serial.printf("err: initGPIO failed ");
    }
}

//----------------------------------------------------------------------------------
// name : setOutput
//----------------------------------------------------------------------------------
bool readGPIOs()
{
    bool bChanged = false;
    for (int i = 0; i < NUM_OF_DATA_FIELDS; i++)
    {
        uint8_t v = digitalRead(g_gpio_pins[i]);
        if ((float)v != g_fValues[i]) // emit only if changed
            bChanged = true;
        g_fValues[i] = (float)v;
    }
    return bChanged;
}

//----------------------------------------------------------------------------------
// name : initWIFI
//----------------------------------------------------------------------------------
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