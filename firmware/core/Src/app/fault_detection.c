#include "fault_detection.h"
#include "led_control.h"
#include "network.h"  // STM32Cube.AI generated header
#include <string.h>
#include "ml_integration.h"
#include "cmsis_os.h"
#include "main.h"

// External variables
extern UART_HandleTypeDef huart1;
extern osMessageQueueId_t faultQueueHandle;

// Pin definitions 
#define HEARTBEAT_IN_PIN GPIO_PIN_13    // PC13
#define HEARTBEAT_IN_PORT GPIOC
#define ML_FAULT_PIN GPIO_PIN_5         // PA5
#define ML_FAULT_PORT GPIOA
#define RESET_OUT_PIN GPIO_PIN_8        // PA8
#define RESET_OUT_PORT GPIOA

// Global system state
system_state_t current_system_state = SYS_STATE_BOOT;

// ML model instance
static ml_model_t ml_model;

// Add the missing fault_handler_task
void fault_handler_task(void *argument) {
    ml_result_t fault;
    
    for(;;) {
        // Wait for fault messages from the queue
        if (osMessageQueueGet(faultQueueHandle, &fault, NULL, osWaitForever) == osOK) {
            handle_detected_fault(&fault);
        }
    }
}

// Your existing heartbeat_monitor_task (FIXED - was missing closing brace)
void heartbeat_monitor_task(void *argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t last_heartbeat_time = xTaskGetTickCount();

    for(;;) {
        // Read heartbeat input from OBC (PC13)
        uint8_t heartbeat_state = HAL_GPIO_ReadPin(HEARTBEAT_IN_PORT, HEARTBEAT_IN_PIN);

        if (heartbeat_state) {
            last_heartbeat_time = xTaskGetTickCount();
            // Update heartbeat LED
            set_led(LED_HEARTBEAT, 1);
        } else {
            set_led(LED_HEARTBEAT, 0);
        }

        // Check for heartbeat timeout (5 seconds)
        if ((xTaskGetTickCount() - last_heartbeat_time) > pdMS_TO_TICKS(5000)) {
            // Trigger OBC fault - Processor hang detection
            ml_result_t fault = {.predicted_class = 1, .confidence = 0.95f, .timestamp = osKernelGetTickCount()};
            handle_detected_fault(&fault);
            last_heartbeat_time = xTaskGetTickCount(); // Reset to prevent continuous triggering
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100)); // 10Hz monitoring
    }
} // This closing brace was missing!

void handle_detected_fault(ml_result_t* fault_result) {
    // Update LED indicators immediately
    update_leds_from_ml_result(fault_result);

    // Signal ML_FAULT to OBC (PA5)
    HAL_GPIO_WritePin(ML_FAULT_PORT, ML_FAULT_PIN, GPIO_PIN_SET);

    // Execute preventive actions based on fault type
    switch(fault_result->predicted_class) {
        case 1: // No heartbeat - OBC reset
            HAL_GPIO_WritePin(RESET_OUT_PORT, RESET_OUT_PIN, GPIO_PIN_SET);
            osDelay(100);
            HAL_GPIO_WritePin(RESET_OUT_PORT, RESET_OUT_PIN, GPIO_PIN_RESET);
            break;

        case 2: // Overcurrent - Power cycling via MOSFETs
            // Assuming MOSFETs on PA1-PA4
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_RESET);
            osDelay(1000); // 1 second power off
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_SET);
            break;

        case 3: // UART timeout - TTC restart
            restart_uart_link();
            break;

        case 4: // Data corruption - TTC recovery
            request_data_retransmission();
            break;
    }

    // Clear ML_FAULT after delay
    osDelay(1000);
    HAL_GPIO_WritePin(ML_FAULT_PORT, ML_FAULT_PIN, GPIO_PIN_RESET);
}

void ml_inference_task(void *argument) {
    // Initialize ML model
    if (!ml_model_init(&ml_model)) {
        Error_Handler();
    }
    
    float ml_input[ML_INPUT_SIZE] = {0};
    float ml_output[ML_OUTPUT_SIZE] = {0};
    
    for(;;) {
        // Collect sensor data for ML input
        collect_ml_input_data(ml_input);
        
        // Run ML inference
        if (ml_model_run_inference(&ml_model, ml_input, ml_output)) {
            // Process ML results
            ml_result_t result = process_ml_output(ml_output);
            
            // If anomaly detected, send to fault handler
            if (result.predicted_class != 0 && result.confidence > 0.7f) {
                osMessageQueuePut(faultQueueHandle, &result, 0, 0);
            }
        }
        osDelay(100); // 10Hz inference rate
    }
}

void collect_ml_input_data(float *input) {
    // TODO: Replace with actual sensor readings
    // For now, use simulated/dummy data
    
    // Read ADC values (simulated)
    input[0] = read_cpu_temperature();
    input[1] = read_voltage_3v3();
    input[2] = read_voltage_5v();
    
    // System metrics (simulated)
    input[3] = get_cpu_usage_percent();
    input[4] = get_memory_usage_percent();
    input[5] = get_heartbeat_rate();
    
    // Fill remaining inputs with zeros or simulated data
    for(int i = 6; i < ML_INPUT_SIZE; i++) {
        input[i] = 0.0f;
    }
}

ml_result_t process_ml_output(float *output) {
    ml_result_t result = {0};
    float max_confidence = 0.0f;
    
    for (int i = 0; i < ML_OUTPUT_SIZE; i++) {
        if (output[i] > max_confidence) {
            max_confidence = output[i];
            result.predicted_class = i;
        }
    }
    
    result.confidence = max_confidence;
    result.timestamp = osKernelGetTickCount();
    
    return result;
}

void restart_uart_link(void) {
    // Re-initialize UART interface
    HAL_UART_DeInit(&huart1);
    osDelay(100);
    HAL_UART_Init(&huart1);
}

void request_data_retransmission(void) {
    // Send retransmission request via UART
    uint8_t retransmit_cmd = 0x55; // Example command
    HAL_UART_Transmit(&huart1, &retransmit_cmd, 1, 100);
}

// Missing sensor function implementations
float read_cpu_temperature(void) {
    // TODO: Implement using internal temperature sensor
    // For STM32H7, you can use the internal temperature sensor
    // This is a placeholder implementation
    return 25.0f; 
}

float read_voltage_3v3(void) {
    // TODO: Implement using ADC to read 3.3V rail
    // This requires proper voltage divider circuit
    return 3.3f;
}

float read_voltage_5v(void) {
    // TODO: Implement using ADC to read 5V rail  
    // This requires proper voltage divider circuit
    return 5.0f;
}

float get_cpu_usage_percent(void) {
    // Simple CPU usage calculation using FreeRTOS
    // This is a basic implementation
    static uint32_t idleCount = 0;
    static uint32_t lastIdleCount = 0;
    static uint32_t lastTick = 0;
    
    uint32_t currentTick = xTaskGetTickCount();
    uint32_t elapsedTicks = currentTick - lastTick;
    
    if (elapsedTicks >= 1000) { // Update every second
        uint32_t currentIdleCount = xTaskGetIdleTaskCount();
        uint32_t idleTicks = currentIdleCount - lastIdleCount;
        
        float usage = 100.0f - ((float)idleTicks / (float)elapsedTicks * 100.0f);
        
        lastIdleCount = currentIdleCount;
        lastTick = currentTick;
        
        return usage > 0 ? usage : 0;
    }
    
    return 45.0f; // Default value
}

float get_memory_usage_percent(void) {
    // Simple heap usage calculation
    // This is a basic implementation
    extern size_t __heap_start, __heap_end;
    size_t total_heap = (size_t)&__heap_end - (size_t)&__heap_start;
    
    struct mallinfo mi = mallinfo();
    float usage = ((float)mi.uordblks / (float)total_heap) * 100.0f;
    
    return usage > 0 ? usage : 65.0f;
}

float get_heartbeat_rate(void) {
    // This would track the actual heartbeat rate from PC13
    // For now, return a simulated value
    static uint32_t last_heartbeat_time = 0;
    static uint32_t heartbeat_interval = 1000; // ms
    
    uint32_t current_time = xTaskGetTickCount();
    uint32_t actual_interval = current_time - last_heartbeat_time;
    
    if (actual_interval > 0) {
        float rate = 1000.0f / (float)actual_interval; // Convert to Hz
        last_heartbeat_time = current_time;
        return rate;
    }
    
    return 1.0f; // Default 1Hz
}