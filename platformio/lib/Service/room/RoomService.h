#ifndef ROOM_SERVICE_H
#define ROOM_SERVICE_H

#ifdef USE_ROOM
#define DHT_DEBUG

#include "../Service.h"
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#define T_PIN V1
#define H_PIN V2
#define HI_PIN V3

template <class T>
class RoomService: public Service<T>
{
private:
  DHT *dht;
  float humidity;
  float temperature;

  void print(float heatIndex)
  {
    Serial.printf("Temp: %.2f, Humi: %.2f, hIdx: %.2f\n",
        temperature, humidity, heatIndex);
  }

public:
  RoomService(BlynkProtocol<T> *blynk): Service<T>(blynk)
  {
    dht = new DHT(DHTPIN, DHTTYPE);
  }

  ~RoomService() {}
  
  void setup()
  {
    dht->begin();
  }

  void readSensors()
  {
    humidity = dht->readHumidity();
    temperature = dht->readTemperature();
  }
  
  void sendBlynk()
  {
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Celsius (isFahreheit = false)
    float heatIndex = dht->computeHeatIndex(temperature, humidity, false);
    // print(heatIndex);

    this->blynk->virtualWrite(T_PIN, temperature);
    this->blynk->virtualWrite(H_PIN, humidity);
    this->blynk->virtualWrite(HI_PIN, heatIndex);
  }

  void sendGCP(CloudIoTCoreMqtt *mqtt)
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
};

#endif // USE_ROOM

#endif // ROOM_SERVICE_H