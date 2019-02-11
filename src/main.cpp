#include <Arduino.h>
#include "MHZ14A.h"
#include "I2cBme280.h"
#include <Wire.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMQTTClient.h>
#include <WiFi.h>
#include "ciotc_config.h"
#include <time.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

CloudIoTCoreDevice device(project_id, location, registry_id, device_id, private_key_str);
CloudIoTCoreMQTTClient client(&device);

const uint32_t BLYNK_INTERVAL = 10000;
const uint32_t GCP_INTERVAL = 60000;

uint32_t lastMsg = 0;
uint32_t lastGcpMsg = 0;

/** Initialize BME280 sensor */
I2cBme280 bme280;

// Initialize CO2 Sensor
MHZ14A co2sensor(2);

// notification
int16_t co2Threshold = 2000;
const int16_t co2NotifyStep = 500;
// error
bool hasError = false;

void notifyCO2(int16_t val)
{
  // send to Blynk
  if (val <= 0 && !hasError)
  {
    Blynk.notify("CO2濃度が取れませんでした！");
    hasError = true;
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
  String payload = String("{\"co2\":") + String(co2) +
                   String(",\"temperature\":") + String(bme280.getTemperature()) +
                   String(",\"humidity\":") + String(bme280.getHumidity()) +
                   String(",\"pressure\":") + String(bme280.getPressure() / 100.0) +
                   String("}");
  //Serial.printf("Payload: %s\n", payload.c_str());
  client.publishTelemetry(payload);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialize bme280 sensor
  Wire.begin();
  bme280.init();

  // Initialize CO2 Sensor
  co2sensor.setup();

  // connect to WiFi&Blynk
  Blynk.begin(blynk_token, ssid, password);

  configTime(0, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  Serial.print("Waiting on time sync... ");
  while (time(nullptr) < 1510644967)
  {
    delay(10);
  }

  struct tm timeInfo;
  getLocalTime(&timeInfo);
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
                timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

  Serial.println("Connecting to mqtt.googleapis.com");
  //client.debugEnable(true);
  client.connectSecure(root_cert);
  // client.setConfigCallback(callback);
}

void loop()
{
  // put your main code here, to run repeatedly:
  Blynk.run();
  int lastState = client.loop();

  if (lastState != 0)
  {
    Serial.println("Error, client state: " + String(lastState));
  }

  uint32_t now = millis();
  if (now - lastMsg > BLYNK_INTERVAL)
  {
    lastMsg = now;

    bool co2Ready = co2sensor.isReady();

    int16_t co2 = co2sensor.readGas();
    //Serial.printf("CO2: %d ppm\n", co2);
    notifyCO2(co2);

    int8_t result = bme280.getSensorData(BME280_ALL);
    if (result == BME280_OK)
    {
      Blynk.virtualWrite(V1, bme280.getTemperature());
      Blynk.virtualWrite(V2, bme280.getHumidity());
      Blynk.virtualWrite(V3, bme280.getPressure() / 100.0);
    }

    if (now - lastGcpMsg < GCP_INTERVAL)
    {
      return;
    }

    lastGcpMsg = now;
    if (lastState == 0 && co2Ready && result == BME280_OK)
    {
      publish(co2);
    }
  }
}