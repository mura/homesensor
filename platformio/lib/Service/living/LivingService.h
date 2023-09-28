#ifndef LIVING_SERVICE_H
#define LIVING_SERVICE_H

#ifdef USE_LIVING

#include "../Service.h"
#include <I2cBme280.h>
#include <MHZ14A.h>

const int MHZ14A_UART_NR = 2;
const int16_t INITIAL_CO2_THRESHOLD = 2000;
const int16_t CO2_NOTIFY_STEP = 500;

template <class T>
class LivingService: public Service<T>
{
private:
  I2cBme280 *bme280;
  MHZ14A *co2sensor;
  bool co2Ready = false;
  bool hasError = false;
  int16_t co2 = -1;
  int16_t co2Threshold = 2000;
  int8_t bme280Result = 0;

public:
  LivingService(BlynkProtocol<T> *blynk): Service<T>(blynk)
  {
    bme280 = new I2cBme280();
    co2sensor = new MHZ14A(MHZ14A_UART_NR);
  }

  ~LivingService() {}
  
  void setup()
  {
    // Initialize bme280 sensor
    bme280->init();

    // Initialize CO2 Sensor
    co2sensor->setup();
  }
  
  void readSensors()
  {
    co2Ready = co2sensor->isReady();
    co2 = co2sensor->readGas();
    bme280Result = bme280->getSensorData(BME280_ALL);
  }
  
  void sendBlynk()
  {
    if (co2 > 0)
    {
      this->blynk->virtualWrite(V0, co2);
    }
    if (bme280Result == BME280_OK)
    {
      // bme280.print();
      this->blynk->virtualWrite(V1, bme280->getTemperature());
      this->blynk->virtualWrite(V2, bme280->getHumidity());
      this->blynk->virtualWrite(V3, bme280->getPressure() / 100.0);
    }
  }
};

#endif // USE_LIVING

#endif // LIVING_SERVICE_H