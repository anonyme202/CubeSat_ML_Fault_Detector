#include "ttc_communication.h"
#include "string.h"
#include "cmsis_os.h"

static ttc_handle_t ttc_handle;
extern UART_HandleTypeDef huart1;

void ttc_communication_init(void) {
    memset(&ttc_handle, 0, sizeof(ttc_handle_t));
    ttc_handle.last_rx_time = osKernelGetTickCount();
    ttc_handle.connection_healthy = 1;
    
    // Start UART reception
    HAL_UART_Receive_IT(&huart1, ttc_handle.rx_buffer, 1);
}

void ttc_receive_callback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        ttc_handle.last_rx_time = osKernelGetTickCount();
        ttc_handle.connection_healthy = 1;
        
        // Process received byte
        if (ttc_handle.rx_index < TTC_BUFFER_SIZE - 1) {
            ttc_handle.rx_index++;
        } else {
            ttc_handle.rx_index = 0; // Wrap around
        }
        
        // Restart reception
        HAL_UART_Receive_IT(&huart1, &ttc_handle.rx_buffer[ttc_handle.rx_index], 1);
    }
}

void ttc_transmit_data(uint8_t *data, uint16_t length) {
    if (length > TTC_BUFFER_SIZE) {
        length = TTC_BUFFER_SIZE;
    }
    
    memcpy(ttc_handle.tx_buffer, data, length);
    HAL_UART_Transmit(&huart1, ttc_handle.tx_buffer, length, 1000);
}

uint8_t ttc_check_connection(void) {
    uint32_t current_time = osKernelGetTickCount();
    
    if ((current_time - ttc_handle.last_rx_time) > TTC_TIMEOUT_MS) {
        ttc_handle.connection_healthy = 0;
        return 0;
    }
    
    return 1;
}

void ttc_monitor_task(void *argument) {
    ttc_communication_init();
    
    // Telemetry transmission counter
    uint32_t telemetry_counter = 0;
    
    for(;;) {
        // Check TTC connection health
        if (!ttc_check_connection()) {
            // TTC connection lost - trigger fault
            ml_result_t fault = {
                .predicted_class = 3, // TTC fault
                .confidence = 0.9f,
                .timestamp = osKernelGetTickCount()
            };
            osMessageQueuePut(faultQueueHandle, &fault, 0, 0);
        }
        
        // Send periodic telemetry (every 5 seconds)
        if (telemetry_counter++ >= 50) { // 50 * 100ms = 5 seconds
            send_telemetry_data();
            telemetry_counter = 0;
        }
        
        osDelay(100); // 10Hz monitoring
    }
}

void send_telemetry_data(void) {
    uint8_t telemetry[32] = {0};
    
    // Populate telemetry data
    telemetry[0] = 0xAA; // Sync byte
    telemetry[1] = current_system_state;
    telemetry[2] = system_reset.global_reset_status;
    
    // Add more telemetry data as needed
    ttc_transmit_data(telemetry, sizeof(telemetry));
}

void restart_uart_link(void) {
    // Re-initialize UART interface
    HAL_UART_DeInit(&huart1);
    osDelay(100);
    HAL_UART_Init(&huart1);
    
    // Restart reception
    ttc_communication_init();
}

void request_data_retransmission(void) {
    uint8_t retransmit_cmd[4] = {0x55, 0xAA, 0x01, 0x00}; // Example command
    ttc_transmit_data(retransmit_cmd, sizeof(retransmit_cmd));
}