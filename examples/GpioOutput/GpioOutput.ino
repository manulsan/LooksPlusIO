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
float g_fValues[NUM_OF_DATA_FIELDS] = {0, 0, 0}; // initial value
LooksPlusIO looksplusIO(DEVICE_SN, NUM_OF_DATA_FIELDS);

uint8_t g_gpio_pins[NUM_OF_DATA_FIELDS] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3}; // gpio pins

//--------------------------------------------------------------------
// name :setup
//--------------------------------------------------------------------
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
    Serial.printf("\n\r----- atCloud365.com  GPIO OUTPUT Example -----\n\r");
}

//--------------------------------------------------------------------
// name : loop
//--------------------------------------------------------------------
void loop()
{
#ifdef _NTP_USED_
    ntpClient.update();
#endif
    unsigned long curMillis = millis();
    if (looksplusIO.isConnected() && (curMillis - g_prevMillis) >= UPLOAD_INTERVAL || g_uploadRequired)
    {
#ifdef _NTP_USED_
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS, ntpClient.getEpochTime());
#else
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS);
#endif
        g_prevMillis = curMillis;
        g_uploadRequired = false;
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

//--------------------------------------------------------------------
// name : onDataCallback
//--------------------------------------------------------------------
void onDataCallback(const char *payload, size_t length)
{
    Serial.printf("onDataCallback, payload=%s\n\r", payload);

    try
    {
        DynamicJsonDocument doc(1024);
        DeserializationError err = deserializeJson(doc, String(payload));
        if (err)
        {
            Serial.printf("deserializeJson() returned ", (const char *)err.f_str());
            return;
        }
        if (doc.containsKey("cmd"))
        {
            auto cmd = doc["cmd"].as<const char *>();
            if (strcmp(cmd, "sync") == 0)
                g_uploadRequired = true;
            else if (strcmp(cmd, "output") == 0)
            {
                auto f = doc["content"]["field"].as<float>();
                auto v = doc["content"]["value"].as<float>();
                setOutput(f, v);
            }
            else if (strcmp(cmd, "output-all") == 0)
            {
                auto v = doc["content"]["value"].as<float>();
                setOutputAll(v);
            }
            else if (strcmp(cmd, "reboot") == 0)
                ESP.restart();
            else
                Serial.printf("cmd %s is not defined\n\r", cmd);
        }
    }
    catch (const std::exception &e)
    {
        Serial.printf("err: %s ", e.what()); // std::cerr << e.what() << '\n';
    }
}

//--------------------------------------------------------------------
// name : setOutput
//--------------------------------------------------------------------
void setOutput(int f, float v)
{
    if (f >= 0 && f < NUM_OF_DATA_FIELDS)
    {
        digitalWrite(g_gpio_pins[f], v);
        g_fValues[f] = v;
    }
    else
        Serial.printf("field out of range\n\r");

    g_uploadRequired = true;
}

//--------------------------------------------------------------------
// name : setOutputAll
//--------------------------------------------------------------------
void setOutputAll(float v)
{
    for (int i = 0; i < NUM_OF_DATA_FIELDS; i++)
        setOutput(i, v);
}

//--------------------------------------------------------------------
// name : initGPIO
//--------------------------------------------------------------------
void initGPIO()
{
    for (uint8_t i = 0; i < NUM_OF_DATA_FIELDS; i++)
    {
        pinMode(g_gpio_pins[i], OUTPUT);
        digitalWrite(g_gpio_pins[i], OFF); // output low, high active g_outputs[GPIO_OUTPUT_PIN] = 0;
        g_fValues[i] = OFF;
    }
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