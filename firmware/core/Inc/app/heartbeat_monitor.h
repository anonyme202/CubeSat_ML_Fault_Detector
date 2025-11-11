#ifndef __HEARTBEAT_MONITOR_H
#define __HEARTBEAT_MONITOR_H

#include "main.h"
#include "cmsis_os.h"

// Heartbeat configuration
#define HEARTBEAT_TIMEOUT_MS 5000
#define HEARTBEAT_SAMPLE_RATE_MS 100  // 10Hz monitoring

// Heartbeat pin definitions
#define HEARTBEAT_IN_PIN GPIO_PIN_13    // PC13 - Input from OBC
#define HEARTBEAT_IN_PORT GPIOC
#define HEARTBEAT_OUT_PIN GPIO_PIN_4    // PC4 - Output (our own heartbeat)
#define HEARTBEAT_OUT_PORT GPIOC

// Function prototypes
void heartbeat_monitor_init(void);
void heartbeat_monitor_task(void *argument);
void generate_heartbeat_signal(void);
uint32_t get_heartbeat_period_ms(void);
uint8_t is_heartbeat_healthy(void);

#endif