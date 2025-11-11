#include "ml_integration.h"
#include "network.h"
#include "network_data.h"
#include "sensor_manager.h"
#include "heartbeat_monitor.h"
#include <string.h>

static ai_error ai_error_code;

uint8_t ml_model_init(ml_model_t *model) {
    ai_error_code = ai_system_init();
    if (ai_error_code != AI_ERROR_NONE) {
        return 0;
    }
    
    ai_error_code = ai_network_create(&model->network, AI_NETWORK_DATA_CONFIG);
    if (ai_error_code != AI_ERROR_NONE) {
        return 0;
    }
    
    model->ai_input = ai_network_inputs_get(model->network, NULL);
    model->ai_output = ai_network_outputs_get(model->network, NULL);
    
    return 1;
}

uint8_t ml_model_run_inference(ml_model_t *model, float *input_data, float *output_data) {
    // Update sensor readings before inference
    update_sensor_readings();
    
    // Prepare ML input features
    collect_ml_input_data(input_data);
    
    // Copy input data to AI buffer
    memcpy(model->ai_input->data, input_data, ML_INPUT_SIZE * sizeof(float));
    
    // Run inference
    ai_error_code = ai_network_run(model->network, model->ai_input, model->ai_output);
    if (ai_error_code != AI_ERROR_NONE) {
        return 0;
    }
    
    // Copy output data
    memcpy(output_data, model->ai_output->data, ML_OUTPUT_SIZE * sizeof(float));
    
    return 1;
}

void collect_ml_input_data(float *input) {
    // Feature 0-2: Voltage readings
    input[0] = read_voltage_3v3();
    input[1] = read_voltage_5v();
    input[2] = read_internal_vref();
    
    // Feature 3-5: Temperature and current
    input[3] = read_cpu_temperature();
    input[4] = read_current_consumption();
    
    // Feature 6-8: Heartbeat metrics
    input[5] = (float)get_heartbeat_period_ms() / 1000.0f; // Convert to seconds
    input[6] = is_heartbeat_healthy() ? 1.0f : 0.0f;
    
    // Feature 9-11: System load metrics (simplified)
    input[7] = get_cpu_usage_percent() / 100.0f;  // Normalized to 0-1
    input[8] = get_memory_usage_percent() / 100.0f; // Normalized to 0-1
    
    // Feature 12-14: Timing metrics
    uint32_t current_tick = osKernelGetTickCount();
    input[9] = (float)(current_tick % 10000) / 10000.0f; // System uptime pattern
    
    // Features 15-31: Additional system metrics and historical patterns
    // These would be filled with more sophisticated monitoring
    for(int i = 10; i < ML_INPUT_SIZE; i++) {
        // Add noise or additional features based on your specific requirements
        input[i] = 0.0f; 
    }
}

// CPU usage calculation using FreeRTOS
float get_cpu_usage_percent(void) {
    static uint32_t last_idle_count = 0;
    static uint32_t last_tick_count = 0;
    
    uint32_t current_idle_count = xTaskGetIdleTaskCount();
    uint32_t current_tick_count = xTaskGetTickCount();
    
    uint32_t idle_ticks = current_idle_count - last_idle_count;
    uint32_t total_ticks = current_tick_count - last_tick_count;
    
    last_idle_count = current_idle_count;
    last_tick_count = current_tick_count;
    
    if (total_ticks > 0) {
        float usage = 100.0f - ((float)idle_ticks * 100.0f / (float)total_ticks);
        return usage > 0 ? usage : 0;
    }
    
    return 25.0f; // Default low usage
}

// Memory usage calculation
float get_memory_usage_percent(void) {
    // Get heap info from FreeRTOS
    size_t free_heap = xPortGetFreeHeapSize();
    size_t total_heap = configTOTAL_HEAP_SIZE;
    
    if (total_heap > 0) {
        float usage = ((float)(total_heap - free_heap) / (float)total_heap) * 100.0f;
        return usage;
    }
    
    return 50.0f; // Default
}

ml_result_t process_ml_output(float *output) {
    ml_result_t result = {0};
    float max_confidence = 0.0f;
    
    // Your ML model should output probabilities for each class:
    // Class 0: Normal operation
    // Class 1: OBC heartbeat fault  
    // Class 2: Power overcurrent
    // Class 3: TTC communication fault
    // Class 4: Data corruption
    
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

void ml_model_deinit(ml_model_t *model) {
    ai_network_destroy(model->network);
    ai_system_deinit();
}