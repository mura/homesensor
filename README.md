# Home Sensor for ESP32
いろいろなセンサーを使って家のセンシングをします

## 使用しているHardware
- ESP32 Dev Kit
- MH-Z14A (CO2センサー)
- BME280 (温度湿度気圧センサー)
- DHT11 (温度湿度センサー)

## 使用しているPlatform
- [Blynk](https://blynk.cc)
  - IoTお手軽アプリ化フレームワーク
- [Google Cloud IoT Core](https://cloud.google.com/iot-core/)
  - クラウドIoT基盤
- [Google Cloud Functions](https://cloud.google.com/functions/)
  - Cloud IoT CoreからのPub/Subの受け取り用
- [Google BigQuery](https://cloud.google.com/bigquery)
  - センサーデータの保存先

## 使用しているSoftware
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO](https://platformio.org/)
  - [VSCodeのExtentions](https://docs.platformio.org/en/latest/ide/vscode.html)として
- [Arduino core for ESP32 WiFi chip](https://github.com/espressif/arduino-esp32)
  - [Get started with Arduino and ESP32-DevKitC: debugging and unit testing](https://docs.platformio.org/en/latest/tutorials/espressif32/arduino_debugging_unit_testing.html#tutorial-espressif32-arduino-debugging-unit-testing)
- 自作MH-Z14Aライブラリ
  - HardwareSerialから読み取る版
  - [MH-Z-CO2-Sensors](https://github.com/tobiasschuerg/MH-Z-CO2-Sensors)をかなり参考にした
- [Bosch Sensortec BME280 sensor driver](https://github.com/BoschSensortec/BME280_driver)
- [Blynk C++ Library](https://github.com/blynkkk/blynk-library)
- [Google Cloud IoT JWT](https://github.com/GoogleCloudPlatform/google-cloud-iot-arduino)
- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
