#include "sensor_manager.h"
#include "adc.h"
#include <string.h>

// Sensor data storage
static float cpu_temperature = 25.0f;
static float voltage_3v3 = 3.3f;
static float voltage_5v = 5.0f;
static float current_consumption = 0.1f;
static float internal_vref = 1.2f;

// Filtering buffers
static float temp_samples[TEMPERATURE_SAMPLE_COUNT] = {0};
static float voltage_samples[VOLTAGE_SAMPLE_COUNT] = {0};
static uint8_t sample_index = 0;

void sensor_manager_init(void) {
    // Initialize ADC if not already done by CubeMX
    HAL_ADC_Start(&hadc1);
    
    // Initialize sample buffers
    memset(temp_samples, 0, sizeof(temp_samples));
    memset(voltage_samples, 0, sizeof(voltage_samples));
}

float read_cpu_temperature(void) {
    return cpu_temperature;
}

float read_voltage_3v3(void) {
    return voltage_3v3;
}

float read_voltage_5v(void) {
    return voltage_5v;
}

float read_current_consumption(void) {
    return current_consumption;
}

float read_internal_vref(void) {
    return internal_vref;
}

void update_sensor_readings(void) {
    // Read internal temperature sensor
    float temp_raw = read_internal_temperature();
    temp_samples[sample_index] = temp_raw;
    
    // Read voltage monitoring
    float vdd_raw = read_vdd_voltage();
    voltage_samples[sample_index] = vdd_raw;
    
    // Apply moving average filter
    float temp_sum = 0, voltage_sum = 0;
    for (int i = 0; i < TEMPERATURE_SAMPLE_COUNT; i++) {
        temp_sum += temp_samples[i];
    }
    for (int i = 0; i < VOLTAGE_SAMPLE_COUNT; i++) {
        voltage_sum += voltage_samples[i];
    }
    
    cpu_temperature = temp_sum / TEMPERATURE_SAMPLE_COUNT;
    voltage_3v3 = voltage_sum / VOLTAGE_SAMPLE_COUNT;
    
    // Estimate other values (you'll need to implement actual sensing)
    voltage_5v = voltage_3v3 * 1.5f;  // Placeholder - implement actual 5V sensing
    current_consumption = 0.1f + (voltage_3v3 / 10.0f);  // Placeholder
    
    sample_index = (sample_index + 1) % TEMPERATURE_SAMPLE_COUNT;
}

// Internal functions for actual ADC reading
float read_internal_temperature(void) {
    // STM32H7 internal temperature sensor conversion
    // TS_CAL1 and TS_CAL2 are calibration values from system memory
    // This is a simplified implementation
    
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint32_t raw_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
    // Convert to temperature (simplified)
    float temperature = ((float)raw_value * 3.3f / 4095.0f - 0.76f) / 0.0025f + 25.0f;
    return temperature;
}

float read_vdd_voltage(void) {
    // Read internal VREF to calculate VDD
    // This is a simplified implementation
    
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint32_t raw_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
    // Convert to voltage (simplified)
    float voltage = (1.2f * 4095.0f) / (float)raw_value;  // VREFINT = 1.2V typical
    return voltage;
}