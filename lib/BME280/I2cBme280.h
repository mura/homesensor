#ifndef I2CBME280_H
#define I2CBME280_H

#include "bme280.h"

#ifdef BME280_FLOAT_ENABLE
#define DATA_UNIT double
#define TEMP_UNIT double
#else
#define DATA_UNIT unit32_t
#define TEMP_UNIT int32_t
#endif

class I2cBme280 {
  private:
    struct bme280_dev dev;
    struct bme280_data comp_data;
    uint8_t settings_sel;

    static void delay_ms(uint32_t period);
    static int8_t read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
    static int8_t write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

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