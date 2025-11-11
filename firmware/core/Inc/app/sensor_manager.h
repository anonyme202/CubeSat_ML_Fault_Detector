#ifndef __SENSOR_MANAGER_H
#define __SENSOR_MANAGER_H

#include "main.h"

// Sensor configuration
#define TEMPERATURE_SAMPLE_COUNT 10
#define VOLTAGE_SAMPLE_COUNT 5

// Function prototypes
void sensor_manager_init(void);
float read_cpu_temperature(void);
float read_voltage_3v3(void);
float read_voltage_5v(void);
float read_current_consumption(void);
float read_internal_vref(void);
void update_sensor_readings(void);

// External ADC handles (from CubeMX)
extern ADC_HandleTypeDef hadc1;

#endif