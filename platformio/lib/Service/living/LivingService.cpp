#ifdef USE_LIVING
#include "LivingService.h"
#include <Arduino.h>
#include <Wire.h>

const int MHZ14A_UART_NR = 2;
const int16_t INITIAL_CO2_THRESHOLD = 2000;
const int16_t CO2_NOTIFY_STEP = 500;

LivingService::LivingService(BlynkProtocol<BlynkArduinoClient> *blynk) : Service(blynk)
{
  bme280 = new I2cBme280();
  co2sensor = new MHZ14A(MHZ14A_UART_NR);
}

LivingService::~LivingService()
{
}

void LivingService::setup()
{
  // Initialize bme280 sensor
  Wire.begin();
  bme280->init();

  // Initialize CO2 Sensor
  co2sensor->setup();
}

void LivingService::readSensors()
{
  co2Ready = co2sensor->isReady();
  co2 = co2sensor->readGas();
  bme280Result = bme280->getSensorData(BME280_ALL);
}

void LivingService::sendBlynk()
{
  notifyCO2();

  if (bme280Result == BME280_OK)
  {
    // bme280.print();
    blynk->virtualWrite(V1, bme280->getTemperature());
    blynk->virtualWrite(V2, bme280->getHumidity());
    blynk->virtualWrite(V3, bme280->getPressure() / 100.0);
  }
}

void LivingService::sendGCP()
{
  if (!co2Ready || bme280Result != BME280_OK) {
    return;
  }
  
  String payload = String("{\"c\":") + String(co2) +
                   String(",\"t\":") + String(bme280->getTemperature()) +
                   String(",\"h\":") + String(bme280->getHumidity()) +
                   String(",\"p\":") + String(bme280->getPressure() / 100.0) +
                   String("}");
  //Serial.printf("Payload: %s\n", payload.c_str());
  mqtt->publishTelemetry(payload.c_str(), payload.length());
}

void LivingService::notifyCO2()
{
  // send to Blynk
  if (co2 <= 0)
  {
    if (co2Ready && !hasError)
    {
      blynk->notify("CO2濃度が取れませんでした！");
    }
    hasError = true;
    return;
  }
  else if (co2 > 0)
  {
    blynk->virtualWrite(V0, co2);
    hasError = false;
  }

  // notify
  if (co2 >= co2Threshold)
  {
    blynk->notify(String(co2) + "ppm を超えたので換気しよう！");
    co2Threshold = CO2_NOTIFY_STEP * ((int)(co2 / CO2_NOTIFY_STEP) + 1);
    Serial.printf("co2Threshold = %d\n", co2Threshold);
  }
  else if (co2 < INITIAL_CO2_THRESHOLD && co2Threshold > INITIAL_CO2_THRESHOLD)
  {
    co2Threshold = INITIAL_CO2_THRESHOLD;
  }
}

#endif // USE_LIVING