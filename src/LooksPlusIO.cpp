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

#ifdef ESP32
void hexdump(const byte *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}
#endif

//------------------------------------------------------------------------------
// name: socketIOEventCallback
// desc: Socket.IO message handler
//------------------------------------------------------------------------------
void LooksPlusIO::socketIOEventCallback(socketIOmessageType_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case sIOtype_DISCONNECT:
        _bConnected = false;
        if (connectCallback != NULL)
            connectCallback(_bConnected);

        if (_bDebug)
            Serial.printf("[IOc] dis-Connected \n");
        break;
    case sIOtype_CONNECT:
        _bConnected = true;
        if (connectCallback != NULL)
            connectCallback(_bConnected);
        if (_bDebug)
            Serial.printf("[IOc] Connected to url: %s\n", payload);
        // socketIO.send(sIOtype_CONNECT, "/"); // join default namespace (no auto join in Socket.IO V3) => alex marked
        break;
    case sIOtype_EVENT:
        if (dataCallback != NULL)
            dataCallback((char *)payload, length);

        if (_bDebug)
            Serial.printf("[IOc] get event: %s\n", payload);
        break;
    case sIOtype_ACK:
        if (_bDebug)
        {
            Serial.printf("[IOc] get ack: %u\n", length);
            hexdump(payload, length);
        }
        break;
    case sIOtype_ERROR:
        if (_bDebug)
        {
            Serial.printf("[IOc] get error: %u\n", length);
            hexdump(payload, length);
        }
        break;
    case sIOtype_BINARY_EVENT:
        if (_bDebug)
        {
            Serial.printf("[IOc] get binary: %u\n", length);
            hexdump(payload, length);
        }
        break;
    case sIOtype_BINARY_ACK:
        if (_bDebug)
        {
            Serial.printf("[IOc] get binary ack: %u\n", length);
            hexdump(payload, length);
        }
        break;
    default:
        Serial.printf("[IOc] default error: %u\n", length);
        break;
    }
}

//------------------------------------------------------------------------------
// name: init,  desc: connect to cloud
//------------------------------------------------------------------------------
void LooksPlusIO::init(std::function<void(const char *payload, size_t length)> dataCb,
                       std::function<void(const bool status)> connectCb, bool debug)
{
    if (_szUrlPath == NULL || strlen(_szUrlPath) >= 64)
        goPanic("Invalid device serial number !!!");

    if (!checkRange(_countOfData, 1, 10))
        goPanic("Invalid field count !!!, field count= [1-10] !!!");

    dataCallback = dataCb ? dataCb : nullptr;
    connectCallback = connectCb ? connectCb : nullptr;

    using std::placeholders::_1;
    using std::placeholders::_2;
    _socketIO.onEvent(std::function<void(socketIOmessageType_t type, uint8_t *payload, size_t length)>(std::bind(&LooksPlusIO::socketIOEventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
    _socketIO.beginSSL(SERVER_URL, SERVER_PORT, _szUrlPath, "atCloud365");
    _socketIO.setReconnectInterval(10000);

    _bDebug = debug;
    if (_bDebug)
        Serial.printf("SERVER_URL=%s  SERVER_PORT=%d _szUrlPath=%s", SERVER_URL, SERVER_PORT, _szUrlPath);
}

//------------------------------------------------------------------------------
// name: send,
// desc: set data to send,  _bHasUpdatedData = true
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

    _bHasUpdatedData = true;
    _createdAt = ts;
    return true;
}

//------------------------------------------------------------------------------
// name: loop
//------------------------------------------------------------------------------
void LooksPlusIO::loop()
{
    if (_bConnected && _bHasUpdatedData)
    {
        DynamicJsonDocument doc(1024);
        JsonArray array = doc.to<JsonArray>();

        array.add("dev-data");
        JsonObject payload = array.createNestedObject();
        if (_createdAt != 0)
            payload["createdAt"] = 0;

        JsonArray content = payload.createNestedArray("content");
        for (uint8_t i = 0; i < _countOfData; i++)
            content.add(_fValues[i]);

        //----------------------------------------------
        String output;
        serializeJson(doc, output);
        Serial.printf("txFrame=%s\n\r", output.c_str());
        _socketIO.sendEVENT(output);

        _bHasUpdatedData = false;
    }
    else
        _socketIO.loop();
}