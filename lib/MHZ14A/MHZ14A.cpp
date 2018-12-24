#include "MHZ14A.h"

const int MHZ14A_RESPONSE_TIME = 60;

const int STATUS_NO_RESPONSE = -2;
const int STATUS_CHECKSUM_MISMATCH = -3;
const int STATUS_INCOMPLETE = -4;
const int STATUS_NOT_READY = -5;

unsigned long lastRequest = 0;

MHZ14A::MHZ14A(int uart_nr): co2Serial(uart_nr) {
  
}

void MHZ14A::setup() {
  co2Serial.begin(9600);
}

bool MHZ14A::isPreHeating() {
  return millis() < (3 * 60 * 1000);
}

bool MHZ14A::isReady() {
  if (isPreHeating()) return false;
  return lastRequest < millis() - MHZ14A_RESPONSE_TIME;
}

int MHZ14A::readGas() {
  const byte cmd[9] = { 0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79 };
  const int len = 9;
  co2Serial.write(cmd, len);
  lastRequest = millis();

  int waited = 0;
  while (co2Serial.available() == 0) {
    delay(100);  // wait a short moment to avoid false reading
    if (waited++ > 10) {
      co2Serial.flush();
      return STATUS_NO_RESPONSE;
    }
  }

  while (co2Serial.available() > 0 && co2Serial.peek() != 0xFF) {
    co2Serial.read();
  }

  byte value[9];
  memset(value, 0, len);
  for (int i = 0; i < len; i++) {
    value[i] = co2Serial.read();
  }

  // convert value
  int val = convertResponseToInt(value);
  //Serial.printf("CO2: %d ppm\n", val) ;

  co2Serial.flush();
  return val;
}

void MHZ14A::calibrateZeroPoint() {
  const byte cmd[9] =  { 0xff, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78 };
  const int len = 9;
  co2Serial.write(cmd, len);
  co2Serial.flush();
  Serial.println("calibration!");
}

int MHZ14A::convertResponseToInt(byte value[9]) {
  // check command
  if (value[0] != 0xff || value[1] != 0x86) {
    return STATUS_CHECKSUM_MISMATCH;
  }
  
  // checksum
  char checksum = 0;
  for (int i = 1; i < 8; i++) {
    checksum += (char)value[i];
  }
  checksum = (0xff - checksum) + 0x01;
  if (checksum != value[8]) {
    Serial.printf("invalid value! checksum:0x%x, lastbit:0x%x\n", checksum, value[8]);
    return STATUS_CHECKSUM_MISMATCH;
  }

  // read value
  return value[2] * 256 + value[3];
}