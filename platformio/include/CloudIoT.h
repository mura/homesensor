#ifndef __CLOUD_IOT_H__
#define __CLOUD_IOT_H__

#include <Client.h>
#include <WiFiClientSecure.h>

#include <MQTT.h>

#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include "ciotc_config.h"
#include "device_config.h"

// !!REPLACEME!!
// The MQTT callback function for commands and configuration updates
// Place your message handler code here.
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

class CloudIoT
{
private:
  WiFiClientSecure *netClient;
  CloudIoTCoreDevice *device;
  MQTTClient *mqttClient;
  unsigned long iss = 0;
  String jwt;

  bool setup()
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      return false;
    }

    if (mqtt == NULL) {
        configTime(0, 0, ntp_primary, ntp_secondary);
        Serial.println("Waiting on time sync...");
        while (time(nullptr) < 1510644967) {
          delay(10);
        }

        netClient = new WiFiClientSecure();
        netClient->setCACert(root_cert);
        mqttClient = new MQTTClient(512);
        mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
        mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);
        mqtt->setUseLts(true);
        mqtt->startMQTT();
    }

    return true;
  }

public:
  CloudIoTCoreMqtt *mqtt;

  CloudIoT(const char *device_id,
           const char *private_key_str)
  {
    device = new CloudIoTCoreDevice(
      project_id, location, registry_id, device_id,
      private_key_str);
  }

  ~CloudIoT() {}

  void run()
  {
    if (!setup())
    {
      return;
    }

    if (!mqttClient->connected()) {
      mqtt->mqttConnect();
    }

    mqtt->loop();
  }

  bool connected()
  {
    return mqtt != NULL && mqttClient->connected();
  }

  String getJwt()
  {
    iss = time(nullptr);
    Serial.println("Refreshing JWT");
    jwt = device->createJWT(iss, jwt_exp_secs);
    return jwt;
  }
};

CloudIoT cloudIoT(device_id, private_key_str);

String getJwt() {
  return cloudIoT.getJwt();
}

#endif //__CLOUD_IOT_H__
