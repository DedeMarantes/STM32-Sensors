/*
 * bme280.h
 *
 *  Created on: Jun 6, 2024
 *      Author: andre
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_
#include "stm32f4xx_hal.h"
#define BME280_ADDR (0x76 << 1)//Endereço do slave sensor BME280

void bme280_reset(I2C_HandleTypeDef *hi2c);
void bme280_read_id(I2C_HandleTypeDef *hi2c);
void bme280_basic_config(I2C_HandleTypeDef *h12c);
int bme280_get_temp(I2C_HandleTypeDef *h12c);

extern float tfine;

#endif /* INC_BME280_H_ */
