#include "heartbeat_monitor.h"
#include "fault_detection.h"
#include "led_control.h"
#include <string.h>

// Heartbeat monitoring variables
static uint32_t last_heartbeat_time = 0;
static uint32_t heartbeat_period = 0;
static uint32_t heartbeat_pulse_count = 0;
static uint32_t last_heartbeat_check = 0;
static uint8_t heartbeat_healthy = 1;

// Our own heartbeat generation
static uint32_t our_heartbeat_last_toggle = 0;
static uint8_t our_heartbeat_state = 0;

void heartbeat_monitor_init(void) {
    last_heartbeat_time = osKernelGetTickCount();
    last_heartbeat_check = osKernelGetTickCount();
    heartbeat_healthy = 1;
    
    // Initialize our heartbeat output
    HAL_GPIO_WritePin(HEARTBEAT_OUT_PORT, HEARTBEAT_OUT_PIN, GPIO_PIN_RESET);
}

void heartbeat_monitor_task(void *argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t current_time;
    uint8_t current_heartbeat_state;
    static uint32_t last_pulse_time = 0;

    heartbeat_monitor_init();

    for(;;) {
        current_time = osKernelGetTickCount();
        
        // Read heartbeat input from OBC (PC13)
        current_heartbeat_state = HAL_GPIO_ReadPin(HEARTBEAT_IN_PORT, HEARTBEAT_IN_PIN);

        // Detect rising edge (heartbeat pulse)
        if (current_heartbeat_state && last_pulse_time == 0) {
            uint32_t pulse_period = current_time - last_heartbeat_time;
            
            // Only count valid pulses (debounce and reasonable period)
            if (pulse_period > 50 && pulse_period < 5000) {
                heartbeat_period = pulse_period;
                last_heartbeat_time = current_time;
                heartbeat_pulse_count++;
                heartbeat_healthy = 1;
                
                // Update heartbeat LED
                set_led(LED_HEARTBEAT, 1);
            }
            last_pulse_time = current_time;
        }
        
        // Clear pulse detection after debounce period
        if (last_pulse_time > 0 && (current_time - last_pulse_time) > 10) {
            last_pulse_time = 0;
            set_led(LED_HEARTBEAT, 0);
        }

        // Check for heartbeat timeout (5 seconds)
        if ((current_time - last_heartbeat_time) > pdMS_TO_TICKS(HEARTBEAT_TIMEOUT_MS)) {
            if (heartbeat_healthy) {
                // Heartbeat just failed - trigger fault
                heartbeat_healthy = 0;
                ml_result_t fault = {
                    .predicted_class = 1, // OBC fault - no heartbeat
                    .confidence = 0.95f, 
                    .timestamp = current_time
                };
                osMessageQueuePut(faultQueueHandle, &fault, 0, 0);
            }
        }

        // Generate our own heartbeat signal (1Hz square wave)
        generate_heartbeat_signal();

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(HEARTBEAT_SAMPLE_RATE_MS));
    }
}

void generate_heartbeat_signal(void) {
    uint32_t current_time = osKernelGetTickCount();
    
    // Generate 1Hz square wave (500ms on, 500ms off)
    if ((current_time - our_heartbeat_last_toggle) >= 500) {
        our_heartbeat_state = !our_heartbeat_state;
        HAL_GPIO_WritePin(HEARTBEAT_OUT_PORT, HEARTBEAT_OUT_PIN, 
                         our_heartbeat_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        our_heartbeat_last_toggle = current_time;
    }
}

uint32_t get_heartbeat_period_ms(void) {
    return heartbeat_period;
}

uint8_t is_heartbeat_healthy(void) {
    return heartbeat_healthy && ((osKernelGetTickCount() - last_heartbeat_time) < HEARTBEAT_TIMEOUT_MS);
}