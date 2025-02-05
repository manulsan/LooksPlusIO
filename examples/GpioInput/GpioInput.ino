/*
    LooksPlusIO Example for GPIO inputs
    Tested with ESP32( Olimax ESP32-GATEWAY B/D)
    No copyrights
*/

#include <LooksPlusIO.h>
#include "def.h"

#ifdef _NTP_USED_
#include <NTPClient.h>
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 0, 60000); // 0 for UTC offset, 60000 for update interval
#endif

bool g_emitRequired = false;
unsigned long g_prevMillis = 0;
float g_fValues[NUM_OF_DATA_FIELDS] = {0, 0}; // data store buf
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
    Serial.printf("----- ARDUINO SKETCH FOR GPIO INPUT -------\n\r");
}

//--------------------------------------------------------------------
// name : loop
//--------------------------------------------------------------------
void loop()
{
#ifdef _NTP_USED_
    ntpClient.update();
#endif
    bool bUpdated = readGPIOs();
    unsigned long curMillis = millis();
    if (looksplusIO.isConnected() && (curMillis - g_prevMillis) >= UPLOAD_INTERVAL || bUpdated || g_emitRequired)
    {
#ifdef _NTP_USED_
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS, ntpClient.getEpochTime());
#else
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS);
#endif
        g_emitRequired = false;
        g_prevMillis = curMillis;
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
// name : setOutput
//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
// name : initGPIO
//--------------------------------------------------------------------
void initGPIO()
{
    for (uint8_t i = 0; i < NUM_OF_DATA_FIELDS; i++)
    {
        pinMode(g_gpio_pins[i], INPUT_PULLUP);
        g_fValues[i] = (float)digitalRead(g_gpio_pins[i]);
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