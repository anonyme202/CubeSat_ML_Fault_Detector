#include "reset_control.h"
#include "cmsis_os.h"

reset_control_t system_reset = {0};

void reset_controller_init(void) {
    // Initialize reset control structure
    memset(&system_reset, 0, sizeof(system_reset));
    
    // Ensure ML_FAULT and RESET_OUT are initially low
    HAL_GPIO_WritePin(ML_FAULT_PORT, ML_FAULT_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RESET_OUT_PORT, RESET_OUT_PIN, GPIO_PIN_RESET);
}

void reset_control_task(void *argument) {
    // Your existing reset_control_task implementation
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // 10Hz update
    
    for(;;) {
        // Check all reset sources (matches SN7432N inputs)
        system_reset.watchdog_reset = check_watchdog_timeout();
        system_reset.power_supervisor_reset = check_power_supervisor();
        system_reset.software_reset = check_software_reset_request();
        system_reset.manual_reset = check_manual_reset_button();

        // Emulate SN7432N OR gate logic in software
        uint8_t combined_reset = system_reset.watchdog_reset ||
                                system_reset.power_supervisor_reset ||
                                system_reset.software_reset ||
                                system_reset.manual_reset;

        if (combined_reset && !system_reset.global_reset_status) {
            // Signal ML_FAULT to OBC before reset
            HAL_GPIO_WritePin(ML_FAULT_PORT, ML_FAULT_PIN, GPIO_PIN_SET);
            
            // Enter reset pending state
            current_system_state = SYS_STATE_RESET_PENDING;
            system_reset.global_reset_status = 1;

            // Execute controlled shutdown
            execute_controlled_shutdown();

            // Trigger hardware reset
            HAL_GPIO_WritePin(RESET_OUT_PORT, RESET_OUT_PIN, GPIO_PIN_SET);
            osDelay(100);
            HAL_GPIO_WritePin(RESET_OUT_PORT, RESET_OUT_PIN, GPIO_PIN_RESET);
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

uint8_t check_watchdog_timeout(void) {
    // Check TPL5010 watchdog status
    return HAL_GPIO_ReadPin(WDOG_DONE_PORT, WDOG_DONE_PIN) == GPIO_PIN_SET;
}

uint8_t check_power_supervisor(void) {
    // Check MAX809L power supervisor
    // This would typically be connected to a GPIO or ADC
    return 0; // Placeholder
}

uint8_t check_software_reset_request(void) {
    // Check if software reset was requested
    return system_reset.software_reset;
}

uint8_t check_manual_reset_button(void) {
    // Check manual reset button (if implemented)
    return 0; // Placeholder
}

void execute_controlled_shutdown(void) {
    // Save critical data to non-volatile memory
    // Close communication channels
    // Set all outputs to safe states
    
    // Flash warning LED
    set_led_blink_rate(LED_WARNING, 100);
    osDelay(500);
}