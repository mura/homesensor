#ifndef I2CBME280_H
#define I2CBME280_H

#include "bme280.h"

#ifdef BME280_FLOAT_ENABLE
#define DATA_UNIT double
#define TEMP_UNIT double
#else
#define DATA_UNIT uint32_t
#define TEMP_UNIT int32_t
#endif

class I2cBme280
{
private:
  struct bme280_dev dev;
  struct bme280_data comp_data;
  uint8_t settings_sel;
  uint8_t dev_addr;

  static void delay_us(uint32_t period, void *intf_ptr);
  static int8_t read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
  static int8_t write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

public:
  I2cBme280(/* args */);
  ~I2cBme280();

  int8_t init();
  int8_t getSensorData(uint8_t sensor_comp);
  DATA_UNIT getPressure();
  TEMP_UNIT getTemperature();
  DATA_UNIT getHumidity();
  int8_t softReset();
};

#endif