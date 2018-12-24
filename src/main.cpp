#define ESP32
#include <Arduino.h>
#include <DHTesp.h>
#include "MHZ14A.h"
#include <CloudIoTCore.h>
#include <CloudIoTCoreMQTTClient.h>
#include <WiFi.h>
#include "ciotc_config.h"
#include <time.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


CloudIoTCoreDevice device(project_id, location, registry_id, device_id, private_key_str);
CloudIoTCoreMQTTClient client(&device);

long lastMsg = 0;

/** Pin number for DHT11 data pin */
const int dhtPin = 4;
/** Initialize DHT sensor */
DHTesp dht;
/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;

// Initialize CO2 Sensor
MHZ14A co2sensor(2);

// notification
bool notified = false;
// error
bool hasError = false;

void notifyCO2(int val) {
  // send to Blynk
  if (val < 0 && !hasError) {
    Blynk.notify("CO2濃度が取れませんでした！");
    hasError = true;
  } else if (val > 0) {
    Blynk.virtualWrite(V0, val);
    hasError = false;
  }

  // notify
  if (val >= 2000 && !notified) {
    Blynk.notify(String(val) + "ppm を超えたので換気しよう！");
    notified = true;
  } else if (val < 2000) {
    notified = false;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);

  // Initialize CO2 Sensor
  co2sensor.setup();

  // connect to WiFi&Blynk
  Blynk.begin(blynk_token, ssid, password);

  configTime(0, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  Serial.print("Waiting on time sync... ");
  while (time(nullptr) < 1510644967) {
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

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  int lastState = client.loop();

  if (lastState != 0) {
    Serial.println("Error, client state: " + String(lastState));
  }

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;

    if (!co2sensor.isReady()) {
      return;
    }

    int co2 = co2sensor.readGas();
    //Serial.printf("CO2: %d ppm\n", co2);
    notifyCO2(co2);

    TempAndHumidity lastValues = dht.getTempAndHumidity();
    //Serial.printf("Temp: %.1f C, Humidity: %.1f %%\n", lastValues.temperature, lastValues.humidity) ;
    Blynk.virtualWrite(V1, lastValues.temperature);
    Blynk.virtualWrite(V2, lastValues.humidity);

    String payload = String("{\"co2\":") + String(co2)
        + String(",\"temperature\":") + String(lastValues.temperature)
        + String(",\"humidity\":") + String(lastValues.humidity)
        + String("}");
    Serial.printf("Payload: %s\n", payload.c_str());
    if (lastState == 0) {
      client.publishTelemetry(payload);
    }
  }
}