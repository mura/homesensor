#ifdef USE_ROOM
#include "RoomService.h"

#define DHTPIN 4
#define DHTTYPE DHT11

#define T_PIN V1
#define H_PIN V2
#define HI_PIN V3

RoomService::RoomService(BlynkProtocol<BlynkArduinoClient> *blynk) : Service(blynk)
{
  dht = new DHT(DHTPIN, DHTTYPE);
}

RoomService::~RoomService()
{
}

void RoomService::setup()
{
  dht->begin();
}

void RoomService::readSensors()
{
  humidity = dht->readHumidity();
  temperature = dht->readTemperature();
}

void RoomService::sendBlynk()
{
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float heatIndex = dht->computeHeatIndex(temperature, humidity, false);

  blynk->virtualWrite(T_PIN, temperature);
  blynk->virtualWrite(H_PIN, humidity);
  blynk->virtualWrite(HI_PIN, heatIndex);
}

void RoomService::sendGCP()
{
  if (isnan(humidity) || isnan(temperature))
  {
    return;
  }

  String payload = String("{\"t\":") + String(temperature) +
                   String(",\"h\":") + String(humidity) +
                   String("}");
  //Serial.printf("Payload: %s\n", payload.c_str());
  mqtt->publishTelemetry(payload);
}

#endif // USE_ROOM