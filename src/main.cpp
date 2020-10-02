#include <Arduino.h>
#include "MHZ14A.h"
#include "I2cBme280.h"
#include <Wire.h>
#include "esp32-mqtt.h"
#include <BlynkSimpleEsp32.h>

const uint32_t BLYNK_INTERVAL = 10000;
const uint32_t GCP_INTERVAL = 60000;

uint32_t lastMsg = 0;
uint32_t lastGcpMsg = 0;

/** Initialize BME280 sensor */
I2cBme280 bme280;

// Initialize CO2 Sensor
MHZ14A co2sensor(2);

// ready
bool co2Ready = false;
// notification
int16_t co2Threshold = 2000;
const int16_t co2NotifyStep = 500;
// error
bool hasError = false;

void notifyCO2(int16_t val)
{
  // send to Blynk
  if (val <= 0)
  {
    if (co2Ready && !hasError) {
      Blynk.notify("CO2濃度が取れませんでした！");
    }
    hasError = true;
    return;
  }
  else if (val > 0)
  {
    Blynk.virtualWrite(V0, val);
    hasError = false;
  }

  // notify
  if (val >= co2Threshold)
  {
    Blynk.notify(String(val) + "ppm を超えたので換気しよう！");
    co2Threshold = co2NotifyStep * ((int)(val / co2NotifyStep) + 1);
    Serial.printf("co2Threshold = %d\n", co2Threshold);
  }
  else if (val < 2000 && co2Threshold > 2000)
  {
    co2Threshold = 2000;
  }
}

void publish(int co2)
{
  String payload = String("{\"c\":") + String(co2) +
                   String(",\"t\":") + String(bme280.getTemperature()) +
                   String(",\"h\":") + String(bme280.getHumidity()) +
                   String(",\"p\":") + String(bme280.getPressure() / 100.0) +
                   String("}");
  //Serial.printf("Payload: %s\n", payload.c_str());
  publishTelemetry(payload);
}

void setup()
{
  Serial.begin(115200);

  // Initialize bme280 sensor
  Wire.begin();
  int result = bme280.init();
  // Serial.printf("bme280.init: %d\n", result);

  // Initialize CO2 Sensor
  co2sensor.setup();

  // connect to WiFi&Google Cloud
  setupCloudIoT();

  // connect to Blynk
  Blynk.config(blynk_token);
}

void loop()
{
  Blynk.run();
  mqttClient->loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected())
  {
    connect();
  }

  uint32_t now = millis();
  if (now - lastMsg > BLYNK_INTERVAL)
  {
    lastMsg = now;

    co2Ready = co2sensor.isReady();

    int16_t co2 = co2sensor.readGas();
    //Serial.printf("CO2: %d ppm\n", co2);
    notifyCO2(co2);

    int8_t result = bme280.getSensorData(BME280_ALL);
    // Serial.printf("result: %d\n", result);
    if (result == BME280_OK)
    {
      // Serial.printf("Temp: %f\n", bme280.getTemperature());
      // Serial.printf("Humi: %f\n", bme280.getHumidity());
      // Serial.printf("Pres: %f\n", bme280.getPressure() / 100.0);
      Blynk.virtualWrite(V1, bme280.getTemperature());
      Blynk.virtualWrite(V2, bme280.getHumidity());
      Blynk.virtualWrite(V3, bme280.getPressure() / 100.0);
    }

    if (now - lastGcpMsg < GCP_INTERVAL)
    {
      return;
    }

    lastGcpMsg = now;
    if (co2Ready && result == BME280_OK)
    {
      publish(co2);
    }
  }
}