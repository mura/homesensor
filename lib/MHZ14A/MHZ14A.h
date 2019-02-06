#ifndef MHZ14A_H
#define MHZ14A_H

#include <Arduino.h>
#include <HardwareSerial.h>

class MHZ14A {
  public:
    MHZ14A(int uart_nr);

    void setup();

    bool isPreHeating();
    bool isReady();

    int16_t readGas();
    void calibrateZeroPoint();
  private:
    HardwareSerial co2Serial;

    int16_t convertResponseToInt(uint8_t value[9]);
};

#endif