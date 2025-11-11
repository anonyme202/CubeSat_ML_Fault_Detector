/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
#define ML_INPUT_SIZE 32    
#define ML_OUTPUT_SIZE 5  
  
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#define ML_MODEL_SIZE 3300
#define HEARTBEAT_TIMEOUT_MS 5000

typedef enum {
    LED_ML_ACTIVE = 0,    // PC0
    LED_FAULT_DBC,        // PC1  
    LED_FAULT_TTC,        // PC2
    LED_WARNING,          // PC3
    LED_HEARTBEAT,        // PC4
    LED_COMM_ACTIVE,      // PC5
    LED_SYS_OK,           // PC9
    LED_COUNT
} led_id_t;

typedef enum {
    SYS_STATE_BOOT = 0,
    SYS_STATE_NORMAL,
    SYS_STATE_ML_ACTIVE,
    SYS_STATE_DATA_COLLECTION, 
    SYS_STATE_WARNING,
    SYS_STATE_OBC_DEGRADATION,
    SYS_STATE_OBC_FAULT,
    SYS_STATE_TTC_FAULT,
    SYS_STATE_CRITICAL,
    SYS_STATE_DEMO,
    SYS_STATE_RESET_PENDING,
    SYS_STATE_COUNT
} system_state_t;

typedef struct {
    uint8_t predicted_class;
    float confidence;
    uint32_t timestamp;
} ml_result_t;

typedef struct {
    uint8_t watchdog_reset;
    uint8_t power_supervisor_reset; 
    uint8_t software_reset;
    uint8_t manual_reset;
    uint8_t global_reset_status;
} reset_control_t;

extern reset_control_t system_reset;
extern system_state_t current_system_state;
extern osMessageQueueId_t faultQueueHandle;


void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
void Error_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
