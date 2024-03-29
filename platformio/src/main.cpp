#define BLYNK_FIRMWARE_VERSION "0.0.1"
#define BLYNK_PRINT Serial

#include <device_config.h>
#include <Arduino.h>
#include <BlynkEdgent.h>
#include <create_service.h>

const uint32_t BLYNK_INTERVAL = 10000;

uint32_t lastMsg = 0;
uint32_t lastGcpMsg = 0;

Service<BlynkArduinoClientSecure<WiFiClientSecure>> *service = create_service(&Blynk);

void setup()
{
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  service->setup();
}

void loop()
{
  BlynkEdgent.run();
  if (!Blynk.connected())
  {
    return;
  }

  delay(10);  // <- fixes some issues with WiFi stability

  uint32_t now = millis();
  if (now - lastMsg > BLYNK_INTERVAL)
  {
    lastMsg = now;

    service->readSensors();
    service->sendBlynk();
  }
}