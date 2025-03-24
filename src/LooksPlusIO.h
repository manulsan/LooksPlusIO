/*********************************************************************************
LooksPlusIO : based on SocketIOclient_Generic from arduino opensource
Updats:
   1.0  : basic connection tests
   1.1  : tested ESP32 , but ESP8266 has ssl bugs
*********************************************************************************/

#if defined(ESP32)
#pragma message "You are working with ESP32 module!"
#elif defined(ESP8266)
#pragma message "You are working with ESP8266 module!"
#else
#pragma message "You are working with un-known  module!"
#endif
#pragma once

#ifndef LOOKSPLUSIO_H
#define LOOKSPLUSIO_H

#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef _socket_io_old_
#include <SocketIoClient.h> // prev version, Ok in ESP32
#else
#include <SocketIOclient.h> // testing in esp8266
#endif
#define _WEBSOCKETS_LOGLEVEL_ 4 // 0-4

#define SERVER_URL "atcloud365.com"
#define SERVER_PORT 443
// #define SOCKETIO_PATH "/api/dev/io/?transport=websocket&sn=%s"
#define SOCKETIO_PATH "/api/dev/io/?sn=%s"

class LooksPlusIO
{
public:
  LooksPlusIO(char *sn, uint8_t fieldCount)
  {
    if (sn != NULL && strlen(sn) < 32)
      sprintf(_szUrlPath, SOCKETIO_PATH, sn);

    _countOfData = fieldCount;
  };

  void goPanic(char *msg)
  {
    Serial.printf("Panic: %s\n\r", msg);
    while (1)
      delay(1000);
  }
  void init(std::function<void(const char *payload, size_t length)> commandCB = nullptr,
            std::function<void(const bool status)> connectCB = nullptr,
            bool debug = false);

  void loop();
  char *getUrlPath() { return _szUrlPath; };
  bool isConnected() { return _bConnected; };
  bool send(float arr[], uint8_t count, unsigned long ts = 0);
  void socketIOEventCallback(socketIOmessageType_t type, uint8_t *payload, size_t length);

private:
  char _szUrlPath[64] = {0};
  bool _bConnected = false;
  bool _bHasUpdatedData = false;
  bool _bDebug = false;
  uint8_t _countOfData = 2;
  unsigned long _createdAt = 0; // epoch time

  float _fValues[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}; // max 10 ea field

  std::function<void(const bool status)> connectCallback = nullptr;
  std::function<void(const char *payload, size_t length)> dataCallback = nullptr;
  bool checkRange(int value, int min, int max) { return (value >= min && value <= max); };

#ifdef _socket_io_old_
  SocketIoClient _socketIO;
#else
  SocketIOclient _socketIO;
#endif
};
#endif