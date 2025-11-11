#ifndef __RESET_CONTROL_H
#define __RESET_CONTROL_H

#include "main.h"

// Hardware pin definitions - EXACT MATCH with hardware doc
#define WDOG_WAKE_PIN GPIO_PIN_6        // PA6
#define WDOG_WAKE_PORT GPIOA
#define WDOG_DONE_PIN GPIO_PIN_7        // PA7  
#define WDOG_DONE_PORT GPIOA
#define RESET_OUT_PIN GPIO_PIN_8        // PA8
#define RESET_OUT_PORT GPIOA
#define ML_FAULT_PIN GPIO_PIN_5         // PA5
#define ML_FAULT_PORT GPIOA

// Function prototypes
void reset_controller_init(void);
void reset_control_task(void *argument);
uint8_t check_watchdog_timeout(void);
uint8_t check_power_supervisor(void);
uint8_t check_software_reset_request(void);
uint8_t check_manual_reset_button(void);
void execute_controlled_shutdown(void);

#endif