#ifndef ROOM_SERVICE_H
#define ROOM_SERVICE_H

#ifdef USE_ROOM

#include "../Service.h"
#include <DHT.h>

class RoomService: public Service
{
private:
  DHT *dht;
  float humidity;
  float temperature;

public:
  RoomService(BlynkProtocol<BlynkArduinoClient> *blynk);
  ~RoomService();
  void setup();
  void readSensors();
  void sendBlynk();
  void sendGCP();
};

#endif // USE_ROOM

#endif // ROOM_SERVICE_H