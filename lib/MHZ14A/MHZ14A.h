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

    int readGas();
    void calibrateZeroPoint();
  private:
    HardwareSerial co2Serial;

    int convertResponseToInt(byte value[9]);
};

#endif