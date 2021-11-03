#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <Blynk/BlynkProtocol.h>
#include <CloudIoTCoreMqtt.h>

// サービスインタフェース
template <class T>
class Service
{
public:
  // 最初に初期化をするメソッド
  virtual void setup();
  // センサーから値を読み取る処理
  virtual void readSensors();
  // Blynkに送信する処理
  virtual void sendBlynk();
  // GCPに送信する処理
  virtual void sendGCP(CloudIoTCoreMqtt *mqtt);

protected:
  BlynkProtocol<T> *blynk;

  Service(BlynkProtocol<T> *blynk)
  {
    this->blynk = blynk;
  }
  
  ~Service() {}
};

#endif // __SERVICE_H__