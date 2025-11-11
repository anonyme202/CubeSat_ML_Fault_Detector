#ifndef __TTC_COMMUNICATION_H
#define __TTC_COMMUNICATION_H

#include "main.h"

#define TTC_BUFFER_SIZE 256
#define TTC_TIMEOUT_MS 2000

typedef struct {
    uint8_t rx_buffer[TTC_BUFFER_SIZE];
    uint8_t tx_buffer[TTC_BUFFER_SIZE];
    uint16_t rx_index;
    uint16_t tx_index;
    uint32_t last_rx_time;
    uint8_t connection_healthy;
} ttc_handle_t;

void ttc_communication_init(void);
void ttc_receive_callback(UART_HandleTypeDef *huart);
void ttc_transmit_data(uint8_t *data, uint16_t length);
uint8_t ttc_check_connection(void);
void ttc_monitor_task(void *argument);
void restart_uart_link(void);

#endif