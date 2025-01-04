/*********************************************************************************
LooksPlusIO : based on SocketIOclient_Generic from arduino opensource
Updats:
   1.0  : basic connection tests
*********************************************************************************/
#if !defined(ESP32)
#error This code is intended to run only on the ESP8266 boards ! Please check your Tools->Board setting.
#endif
#pragma once

#ifndef LOOKSPLUSIO_H
#define LOOKSPLUSIO_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SocketIoClient.h> // #define _WEBSOCKETS_LOGLEVEL_ 0 // 0-4

#define SERVER_URL "damosys.com"
#define SERVER_PORT 9000

class LooksPlusIO
{
public:
  LooksPlusIO(char *sn)
  {
    if (sn != NULL && strlen(sn) < 32)
      sprintf(_szUrlPath, "/api/dev/io/?transport=websocket&sn=%s", sn);
  };
  void init(std::function<void(const char *payload, size_t length)> userCB);
  void loop();
  bool send(float arr[], uint8_t count, unsigned long ts = 0);
  bool isConnected() { return _bConnected; };
  void setConnectionStatus(bool status) { _bConnected = status; };

private:
  char _szUrlPath[128] = {NULL};
  bool _bModified = false;
  bool _bConnected = false;
  unsigned long _createdAt = 0; // epoch time

  // this is the number of fields, it cab be changed after server connection
  uint8_t _countOfData = 2;
  float _fValues[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

  void onConnected(const char *payload, size_t length);
  void onDisconnected(const char *payload, size_t length);

  SocketIoClient _socketIO;
};
#endif