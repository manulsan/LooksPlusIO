/* Copyright (C) 2022 everybody */

#include "LooksPlusIO.h"

//------------------------------------------------------------------------------
// name: init,  desc: connect to cloud
//------------------------------------------------------------------------------
void LooksPlusIO::init(std::function<void(const char *payload, size_t length)> userCB)
{
    //---------------------------------------------
    // data validation check
    if (_szUrlPath == NULL && strlen(_szUrlPath) >= 32)
        goPanic("Invalid device serial number");

    if (_countOfData <= 0 || _countOfData > 10)
        goPanic("Invalid field count !!!, field count= 1-10");

    //---------------------------------------------
    _bConnected = false;
    using std::placeholders::_1;
    using std::placeholders::_2;
    _socketIO.on("connect", std::bind(&LooksPlusIO::onConnected, this, std::placeholders::_1, std::placeholders::_2));
    _socketIO.on("disconnect", std::bind(&LooksPlusIO::onDisconnected, this, std::placeholders::_1, std::placeholders::_2));

    if (userCB != NULL)
        _socketIO.on("app-cmd", userCB);

    // Serial.printf("SERVER_URL=%s  SERVER_PORT=%d _szUrlPath=%s", SERVER_URL, SERVER_PORT, _szUrlPath);
    _socketIO.beginSSL(SERVER_URL, SERVER_PORT, _szUrlPath);
}

//------------------------------------------------------------------------------
// name: onConnected,   desc: connected callback
//------------------------------------------------------------------------------
void LooksPlusIO::onConnected(const char *payload, size_t length)
{
    _bConnected = true;
    Serial.println("onConnected, message: ");
}

//------------------------------------------------------------------------------
// name: onDisconnected,    desc: disconnected callback
//------------------------------------------------------------------------------
void LooksPlusIO::onDisconnected(const char *payload, size_t length)
{
    _bConnected = false;
    Serial.println("onDisconnected, message: ");
}

//------------------------------------------------------------------------------
// name: send,  desc: send data to cloud
//------------------------------------------------------------------------------
bool LooksPlusIO::send(float fArr[], uint8_t count, unsigned long ts)
{
    if (count != _countOfData)
    {
        Serial.printf("size not match, _countOfData=%d \n", _countOfData);
        return false;
    }
    for (uint8_t i = 0; i < count; i++)
        _fValues[i] = fArr[i];

    _bDataModified = true;
    _createdAt = ts;
    return true;
}

//------------------------------------------------------------------------------
// name: loop,  desc: send data to cloud
//------------------------------------------------------------------------------
void LooksPlusIO::loop()
{
    if (_bConnected && _bDataModified)
    {
        DynamicJsonDocument doc(1024);
        JsonObject root = doc.to<JsonObject>();

        JsonArray array = doc.createNestedArray("content");
        for (uint8_t i = 0; i < _countOfData; i++)
            array.add(_fValues[i]);

        if (_createdAt != 0)
            root["createdAt"] = _createdAt;

        String output;
        serializeJson(doc, output);

        Serial.printf("topic=dev-data, payload=%s\n\r", output.c_str());
        _socketIO.emit("dev-data", output.c_str());

        _bDataModified = false;
    }
    else
        _socketIO.loop();
}