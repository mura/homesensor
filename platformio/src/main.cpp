#include <Arduino.h>
#include <BlynkSimpleEsp32.h>
#include <esp32-mqtt.h>
#include <create_service.h>
#include <device_config.h>

const uint32_t BLYNK_INTERVAL = 10000;
const uint32_t GCP_INTERVAL = 60000;

uint32_t lastMsg = 0;
uint32_t lastGcpMsg = 0;

Service *service = create_service(&Blynk);

void setup()
{
  Serial.begin(115200);

  // connect to WiFi&Google Cloud
  setupCloudIoT(device_id, private_key_str);

  // connect to Blynk
  Blynk.config(blynk_token);

  // setup service
  service->setupMqtt(mqtt);
  service->setup();
}

void loop()
{
  Blynk.run();
  mqtt->loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected())
  {
    connect();
  }

  uint32_t now = millis();
  if (now - lastMsg > BLYNK_INTERVAL)
  {
    lastMsg = now;

    service->readSensors();
    service->sendBlynk();

    if (now - lastGcpMsg < GCP_INTERVAL)
    {
      return;
    }

    lastGcpMsg = now;
    service->sendGCP();
  }
}