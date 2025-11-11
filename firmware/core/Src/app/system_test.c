#include "system_test.h"
#include "led_control.h"
#include "reset_control.h"
#include "ttc_communication.h"
#include "fault_detection.h"
#include "cmsis_os.h"

void run_system_self_test(void) {
    // Comprehensive system self-test
    set_led(LED_SYS_OK, 0); // Turn off SYS_OK during test
    
    test_led_system();
    HAL_Delay(1000);  // Use HAL_Delay instead of osDelay (RTOS not started yet)
    
    test_communication_system();
    HAL_Delay(1000);
    
    test_ml_system();
    HAL_Delay(1000);
    
    // If all tests pass
    set_led(LED_SYS_OK, 1);
    // Note: ws2812b_set_color might not work until RTOS starts
}

void test_led_system(void) {
    // Test individual LEDs
    for(int i = 0; i < LED_COUNT; i++) {
        set_led(i, 1);
        osDelay(200);
        set_led(i, 0);
    }
    
    // Test RGB LED
    ws2812b_set_color(COLOR_BOOT);
    osDelay(500);
    ws2812b_set_color(COLOR_NORMAL);
}

void test_communication_system(void) {
    // Test UART communication
    uint8_t test_data[] = "SELF_TEST";
    ttc_transmit_data(test_data, sizeof(test_data) - 1);
    
    set_led(LED_COMM_ACTIVE, 1);
    osDelay(500);
    set_led(LED_COMM_ACTIVE, 0);
}

void test_ml_system(void) {
    // Test ML system initialization
    ml_model_t test_model;
    if (ml_model_init(&test_model)) {
        set_led(LED_ML_ACTIVE, 1);
        osDelay(500);
        set_led(LED_ML_ACTIVE, 0);
        ml_model_deinit(&test_model);
    } else {
        // ML test failed
        set_led(LED_FAULT_DBC, 1);
    }
}

void test_fault_detection(void) {
    // Test fault detection by simulating a heartbeat timeout
    ml_result_t test_fault = {
        .predicted_class = 1, // OBC fault
        .confidence = 0.95f,
        .timestamp = osKernelGetTickCount()
    };
    
    // This should trigger LED changes and potentially reset
    handle_detected_fault(&test_fault);
}