#ifndef __FAULT_DETECTION_H
#define __FAULT_DETECTION_H

#include "main.h"
#include "cmsis_os.h"

// Heartbeat monitor defines
#define HEARTBEAT_IN_PIN GPIO_PIN_13    // PC13
#define HEARTBEAT_IN_PORT GPIOC

// ML Fault pin
#define ML_FAULT_PIN GPIO_PIN_5         // PA5
#define ML_FAULT_PORT GPIOA

// Reset output pin  
#define RESET_OUT_PIN GPIO_PIN_8        // PA8
#define RESET_OUT_PORT GPIOA

// Function prototypes
void heartbeat_monitor_task(void *argument);
void ml_inference_task(void *argument);
void fault_handler_task(void *argument);
void handle_detected_fault(ml_result_t* fault_result);
void restart_uart_link(void);
void request_data_retransmission(void);

// Sensor function prototypes
float read_cpu_temperature(void);
float read_voltage_3v3(void);
float read_voltage_5v(void);
float get_cpu_usage_percent(void);
float get_memory_usage_percent(void);
float get_heartbeat_rate(void);

#endif