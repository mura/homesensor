#ifndef LIVING_SERVICE_H
#define LIVING_SERVICE_H

#ifdef USE_LIVING

#include "../Service.h"
#include <I2cBme280.h>
#include <MHZ14A.h>

class LivingService: public Service
{
private:
  I2cBme280 *bme280;
  MHZ14A *co2sensor;
  bool co2Ready = false;
  bool hasError = false;
  int16_t co2 = -1;
  int16_t co2Threshold = 2000;
  int8_t bme280Result = 0;
  void notifyCO2();

public:
  LivingService(BlynkProtocol<BlynkArduinoClient> *blynk);
  ~LivingService();
  void setup();
  void readSensors();
  void sendBlynk();
  void sendGCP();
};

#endif // USE_LIVING

#endif // LIVING_SERVICE_H