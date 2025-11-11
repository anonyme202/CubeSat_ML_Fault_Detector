#ifndef __WATCHDOG_MANAGER_H
#define __WATCHDOG_MANAGER_H

#include "main.h"

// Pin definitions for TPL5010 Watchdog
#define WDOG_WAKE_PIN GPIO_PIN_6        // PA6 - Output to watchdog
#define WDOG_WAKE_PORT GPIOA
#define WDOG_DONE_PIN GPIO_PIN_7        // PA7 - Input from watchdog
#define WDOG_DONE_PORT GPIOA

// Function prototypes
void watchdog_manager_init(void);
void watchdog_manager_task(void *argument);
uint8_t watchdog_check_timeout(void);
void watchdog_trigger_reset(void);
uint32_t watchdog_get_time_since_last_ping(void);

#endif