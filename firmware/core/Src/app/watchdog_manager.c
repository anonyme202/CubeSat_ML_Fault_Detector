#include "watchdog_manager.h"
#include "cmsis_os.h"
#include "main.h"  // Add this for system_reset and other definitions

// TPL5010 Watchdog management
static uint32_t last_watchdog_ping = 0;
static uint8_t watchdog_initialized = 0;

// External variables
extern reset_control_t system_reset;  // Defined in main.c

// Pin definitions (add these if not in your header)
#ifndef WDOG_WAKE_PIN
#define WDOG_WAKE_PIN GPIO_PIN_6        // PA6
#define WDOG_WAKE_PORT GPIOA
#endif

#ifndef WDOG_DONE_PIN  
#define WDOG_DONE_PIN GPIO_PIN_7        // PA7
#define WDOG_DONE_PORT GPIOA
#endif

void watchdog_manager_init(void) {
    // Initialize watchdog wake pin as output
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = WDOG_WAKE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(WDOG_WAKE_PORT, &GPIO_InitStruct);
    
    // Initialize watchdog done pin as input
    GPIO_InitStruct.Pin = WDOG_DONE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WDOG_DONE_PORT, &GPIO_InitStruct);
    
    // Set initial state
    HAL_GPIO_WritePin(WDOG_WAKE_PORT, WDOG_WAKE_PIN, GPIO_PIN_RESET);
    
    watchdog_initialized = 1;
    last_watchdog_ping = osKernelGetTickCount();
}

void watchdog_manager_task(void *argument) {
    if (!watchdog_initialized) {
        watchdog_manager_init();
    }
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(500); // 2Hz update
    
    for(;;) {
        // Toggle WDOG_WAKE pin to keep watchdog alive
        HAL_GPIO_TogglePin(WDOG_WAKE_PORT, WDOG_WAKE_PIN);
        last_watchdog_ping = xTaskGetTickCount();
        
        // Check if watchdog has timed out (WDOG_DONE pin high indicates timeout)
        if (watchdog_check_timeout()) {
            // Watchdog timeout detected - trigger system reset
            system_reset.watchdog_reset = 1;
            // You might want to log this or take additional action
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

uint8_t watchdog_check_timeout(void) {
    // WDOG_DONE pin goes HIGH when watchdog times out
    return HAL_GPIO_ReadPin(WDOG_DONE_PORT, WDOG_DONE_PIN) == GPIO_PIN_SET;
}

void watchdog_trigger_reset(void) {
    // Stop toggling the watchdog to force a timeout and reset
    // This function can be called to manually trigger a watchdog reset
    if (watchdog_initialized) {
        // Stop the watchdog keepalive by not toggling the pin
        // The watchdog will timeout and reset the system
        // You might want to set a flag or take other actions before reset
        system_reset.watchdog_reset = 1;
    }
}

// Optional: Function to get time since last successful watchdog ping
uint32_t watchdog_get_time_since_last_ping(void) {
    if (watchdog_initialized) {
        return osKernelGetTickCount() - last_watchdog_ping;
    }
    return 0;
}