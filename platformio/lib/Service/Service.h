#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <CloudIoTCoreMqtt.h>

// サービスインタフェース
class Service
{
public:
  // MQTTの初期化
  void setupMqtt(CloudIoTCoreMqtt *mqtt)
  {
    this->mqtt = mqtt;
  }
  // 最初に初期化をするメソッド
  virtual void setup();
  // センサーから値を読み取る処理
  virtual void readSensors();
  // Blynkに送信する処理
  virtual void sendBlynk();
  // GCPに送信する処理
  virtual void sendGCP();

protected:
  BlynkProtocol<BlynkArduinoClient> *blynk;
  CloudIoTCoreMqtt *mqtt;

  Service(BlynkProtocol<BlynkArduinoClient> *blynk)
  {
    this->blynk = blynk;
  }
  ~Service() {}

};

#endif // __SERVICE_H__