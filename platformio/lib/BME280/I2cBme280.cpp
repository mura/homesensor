#include "I2cBme280.h"
#include <Arduino.h>
#include <Wire.h>

I2cBme280::I2cBme280(/* args */)
{
  dev_addr = BME280_I2C_ADDR_PRIM;
  dev.intf_ptr = &dev_addr;
  dev.intf = BME280_I2C_INTF;
  dev.read = read;
  dev.write = write;
  dev.delay_us = delay_us;

  dev.settings.osr_t = BME280_OVERSAMPLING_2X;
  dev.settings.osr_h = BME280_OVERSAMPLING_1X;
  dev.settings.osr_p = BME280_OVERSAMPLING_16X;
  dev.settings.filter = BME280_FILTER_COEFF_OFF;
  dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

  settings_sel = BME280_OSR_PRESS_SEL;
  settings_sel |= BME280_OSR_TEMP_SEL;
  settings_sel |= BME280_OSR_HUM_SEL;
  settings_sel |= BME280_STANDBY_SEL;
  settings_sel |= BME280_FILTER_SEL;
}

I2cBme280::~I2cBme280()
{
}

int8_t I2cBme280::init()
{
  Wire.begin();
  
  int8_t rslt = BME280_OK;
  rslt = bme280_init(&dev);
  if (rslt != BME280_OK)
  {
    Serial.printf("Failed bme280_init: %d\n", rslt);
    return rslt;
  }
  rslt = bme280_set_sensor_settings(settings_sel, &dev);
  if (rslt != BME280_OK)
  {
    Serial.printf("Failed bme280_set_sensor_settings: %d\n", rslt);
    return rslt;
  }
  rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);
  if (rslt != BME280_OK)
  {
    Serial.printf("Failed bme280_set_sensor_mode: %d\n", rslt);
  }
  return rslt;
}

int8_t I2cBme280::getSensorData(uint8_t sensor_comp)
{
  return bme280_get_sensor_data(sensor_comp, &comp_data, &dev);
}

DATA_UNIT I2cBme280::getPressure()
{
  return comp_data.pressure;
}

TEMP_UNIT I2cBme280::getTemperature()
{
  return comp_data.temperature;
}

DATA_UNIT I2cBme280::getHumidity()
{
  return comp_data.humidity;
}

int8_t I2cBme280::softReset()
{
  return bme280_soft_reset(&dev);
}

void I2cBme280::delay_us(uint32_t period, void *intf_ptr)
{
  delayMicroseconds(period);
}

int8_t I2cBme280::read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

  /*
   * The parameter intf_ptr can be used as a variable to store the I2C address of the device
   */

  /*
   * Data on the bus should be like
   * |------------+---------------------|
   * | I2C action | Data                |
   * |------------+---------------------|
   * | Start      | -                   |
   * | Write      | (reg_addr)          |
   * | Stop       | -                   |
   * | Start      | -                   |
   * | Read       | (reg_data[0])       |
   * | Read       | (....)              |
   * | Read       | (reg_data[len - 1]) |
   * | Stop       | -                   |
   * |------------+---------------------|
   */
  uint8_t dev_addr = *(uint8_t*)intf_ptr;
  Wire.beginTransmission(dev_addr);
  Wire.write(reg_addr);
  Wire.endTransmission();
  rslt = Wire.requestFrom(dev_addr, (uint8_t)len);
  if (rslt == 0)
  {
    return 1;
  }

  for (int i = 0; i < len; i++)
  {
    reg_data[i] = Wire.read();
  }

  return BME280_OK;
}

int8_t I2cBme280::write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

  /*
   * The parameter intf_ptr can be used as a variable to store the I2C address of the device
   */

  /*
   * Data on the bus should be like
   * |------------+----------------------|
   * | I2C action | Data                 |
   * |------------+----------------------|
   * | Start      | -                    |
   * | Write      | (reg_addr)           |
   * | Write      | (reg_data[0])        |
   * | Write      | (reg_addr + 1)       |
   * | Write      | (reg_data[1])        |
   * | Write      | (....)               |
   * | Write      | (reg_addr + len - 1) |
   * | Write      | (reg_data[len - 1])  |
   * | Stop       | -                    |
   * |------------+----------------------|
   */
  uint8_t dev_addr = *(uint8_t*)intf_ptr;
  Wire.beginTransmission(dev_addr);
  for (int i = 0; i < len; i++)
  {
    Wire.write(reg_addr + i);
    Wire.write(reg_data[i]);
  }
  rslt = Wire.endTransmission();
  return rslt;
}

void I2cBme280::print() 
{
#ifdef BME280_FLOAT_ENABLE
  Serial.printf("Temp: %.2f, Humi: %.2f, Pres: %.2f\n",
                comp_data.temperature, comp_data.humidity, comp_data.temperature / 100.0);
#else
  Serial.printf("Temp: %.2f, Humi: %.2f, Pres: %.2f\n",
                comp_data.temperature / 100.0, comp_data.humidity / 1024.0, comp_data.temperature / 100.0);
#endif
}