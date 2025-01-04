/*
 LooksPlusIO Example for GPIO outputs
    This example is for ESP8266, ESP32, etc.
        tested with ESP32 only

*/

#include <LooksPlusIO.h>
#define WIFI_SSID "DAMOSYS"  // replace with "YOUR_WIFI_SSID"
#define WIFI_PASS "damo8864" // replace with "YOUR_WIFI_PASSWORD"

#define DEVICE_SN "000303FF0024110000001" // replace with your device serial number
#define UPLOAD_INTERVAL 5000

// #define _NTP_USED_
#ifdef _NTP_USED_
// if NTP used, "data created time" is used, if not server side time is used
#include <NTPClient.h>
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 0, 60000); // 0 for UTC offset, 60000 for update interval
#endif

#define NUM_OF_DATA_FIELDS 2 // number of sensors : MAX 10, default 2

float g_fValues[NUM_OF_DATA_FIELDS] = {0, 0}; // sensor values array
unsigned long prevMillis = 0;
bool g_emitRequired = false;

LooksPlusIO looksplusIO(DEVICE_SN);
void commandCB(const char *payload, size_t length); // callback func that is received from apps command
void initWifi();
void initGPIO();

#define OFF 0
#define ON 1
#define GPIO_OUTPUT_PIN 2 // replace with your gpio pin
#define GPIO_OUTPUT_PIN 3 // replace with your gpio pin
uintt_t g_gpio_pin[NUM_OF_DATA_FIELDS] = {GPIO_OUTPUT_PIN, GPIO_OUTPUT_PIN};

void setOutput(int f, float v);
void setOutputAll(float v);

//------------------------------------------------------------------------------
// name :setup
void setup()
{
    Serial.begin(115200);

    initWIFI();
    initGPIO();

#ifdef _NTP_USED_
    ntpClient.begin();
    ntpClient.update();
#endif

    looksplusIO.init(commandCB);
}

//------------------------------------------------------------------------------
// name : loop
void loop()
{
#ifdef _NTP_USED_
    ntpClient.update();
#endif
    unsigned long curMillis = millis();
    if (curMillis - prevMillis >= UPLOAD_INTERVAL || g_emitRequired)
    {
#ifdef _NTP_USED_
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS, ntpClient.getEpochTime());
#else
        looksplusIO.send(g_fValues, NUM_OF_DATA_FIELDS);
#endif
        prevMillis = curMillis;
        g_emitRequired = false
    }
    looksplusIO.loop();
}

//------------------------------------------------------------------------------
// name : commandCB
void commandCB(const char *payload, size_t length)
{
    Serial.printf("commandCB, payload=%s\n\r", payload);

    const int capacity = JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + 4 * JSON_OBJECT_SIZE(1);
    StaticJsonDocument<capacity> doc;
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
    {
        //  reboot();
    }

    else
        Serial.printf("cmd %s is not defined\n\r", cmd);
}

//------------------------------------------------------------------------------
// name : setOutput
void setOutput(int f, float v)
{
    if (f > 0 && f < NUM_OF_DATA_FIELDS)
    {
        digitalWrite(g_gpio_pin[f], v);
        g_fValues[f] = v;
    }
    else
        Serial.printf("field out of range\n\r");

    g_emitRequired = true;
}

//------------------------------------------------------------------------------
// name : setOutputAll
void setOutputAll(float v)
{
    for (int i = 0; i < NUM_OF_DATA_FIELDS; i++)
        setOutput(i, v);
}

//------------------------------------------------------------------------------
// name : initGPIO
void initGPIO()
{
    for (uint8_t i = 0; i < NUM_OF_DATA_FIELDS; i++)
    {
        pinMode(g_gpio_pin[i], OUTPUT);
        digitalWrite(g_gpio_pin[i], OFF); // output low, high active g_outputs[GPIO_OUTPUT_PIN] = 0;
        g_fValues[i] = OFF;
    }
}

//------------------------------------------------------------------------------
// name : initWIFI
void initWIFI()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("WiFi connected\n\rIP address: ");
    Serial.println(WiFi.localIP());
}