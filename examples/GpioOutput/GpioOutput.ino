/*
    LooksPlusIO Example for GPIO outputs
    Tested with ESP32( Olimax ESP32-GATEWAY B/D)
    No copyrights
*/

#include <LooksPlusIO.h>
#include "def.h"

#ifdef _NTP_USED_
// if NTP used, "data created time" is used, if not server side time is used
#include <NTPClient.h>
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 0, 60000); // 0 for UTC offset, 60000 for update interval
#endif

bool g_emitRequired = false;
unsigned long g_prevMillis = 0;
float g_fValues[NUM_OF_DATA_FIELDS] = {0, 0, 0}; // initial value
uint8_t g_gpio_pins[NUM_OF_DATA_FIELDS] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
LooksPlusIO looksplusIO(DEVICE_SN, NUM_OF_DATA_FIELDS);

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

    looksplusIO.init(onCommandCallback, onConnectCallback);
    Serial.printf("----- ARDUINO SKETCH FOR GPIO OUTPUT -----\n\r");
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
    if (looksplusIO.isConnected() && (curMillis - g_prevMillis) >= UPLOAD_INTERVAL || g_emitRequired)
    {
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
    {
        Serial.printf("deserializeJson() returned ", (const char *)err.f_str());
        return;
    }
    auto cmd = doc["cmd"].as<const char *>();
    if (strcmp(cmd, "sync") == 0)
        g_emitRequired = true;
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

    g_emitRequired = true;
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