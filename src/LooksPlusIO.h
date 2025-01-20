/*********************************************************************************
LooksPlusIO : based on SocketIOclient_Generic from arduino opensource
Updats:
   1.0  : basic connection tests
   1.1  : tested ESP32 , but ESP8266 has ssl bugs
*********************************************************************************/

#if defined(ESP32)
#pragma message "ESP32 B/D is working!"
#elif defined(ESP8266)
#pragma message "ESP8266 is not working well, has bus!"
#error ESP8266 has ssl problem, stop compiling
#else
#error This code is intended to run only on the ESP8266/ESP32 boards ! Please check your Tools->Board setting.
#endif
#pragma once

#ifndef LOOKSPLUSIO_H
#define LOOKSPLUSIO_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SocketIoClient.h>

#define _WEBSOCKETS_LOGLEVEL_ 0 // 0-4

// #define SERVER_URL "atcloud365.com" // #define SERVER_URL "1.218.138.131"
// #define SERVER_PORT 443

#define SERVER_URL "damosys.com" // #define SERVER_URL "1.218.138.131"
#define SERVER_PORT 9000

#define SOCKETIO_PATH "/api/dev/io/?transport=websocket&sn=%s"
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
    Serial.println("msg");
    while (1)
      delay(1000);
  }
  void init(std::function<void(const char *payload, size_t length)> userCB);
  void loop();
  bool send(float arr[], uint8_t count, unsigned long ts = 0);
  bool isConnected() { return _bConnected; };
  void setConnectionStatus(bool status) { _bConnected = status; };

private:
  char _szUrlPath[128] = {0};
  bool _bDataModified = false;
  bool _bConnected = false;
  unsigned long _createdAt = 0; // epoch time

  uint8_t _countOfData = 2;
  float _fValues[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

  void onConnected(const char *payload, size_t length);
  void onDisconnected(const char *payload, size_t length);

  SocketIoClient _socketIO;
};
#endif