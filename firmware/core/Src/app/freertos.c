/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "led_control.h"
#include "reset_control.h"
#include "heartbeat_monitor.h"
#include "fault_detection.h"
#include "ttc_communication.h"
#include "watchdog_manager.h"
#include "ml_integration.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

// Task handles (if you need them)
osThreadId_t heartbeatTaskHandle;
osThreadId_t mlTaskHandle;
osThreadId_t ledTaskHandle;
osThreadId_t faultTaskHandle;
osThreadId_t resetTaskHandle;
osThreadId_t ttcTaskHandle;
osThreadId_t watchdogTaskHandle;

// Task attributes
osThreadAttr_t heartbeat_monitor_attributes = {
    .name = "HeartbeatMonitor",
    .stack_size = 1024,
    .priority = osPriorityNormal,
};

osThreadAttr_t ml_inference_attributes = {
    .name = "MLInference", 
    .stack_size = 4096,
    .priority = osPriorityNormal,
};

osThreadAttr_t led_controller_attributes = {
    .name = "LEDController",
    .stack_size = 1024,
    .priority = osPriorityLow,
};

osThreadAttr_t fault_handler_attributes = {
    .name = "FaultHandler",
    .stack_size = 1536,
    .priority = osPriorityRealtime,
};

osThreadAttr_t reset_control_attributes = {
    .name = "ResetControl",
    .stack_size = 1024, 
    .priority = osPriorityHigh,
};

osThreadAttr_t ttc_monitor_attributes = {
    .name = "TTCMonitor",
    .stack_size = 2048,
    .priority = osPriorityNormal,
};

osThreadAttr_t watchdog_manager_attributes = {
    .name = "WatchdogManager",
    .stack_size = 1024,
    .priority = osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  
  // Create message queue for fault handling
  faultQueueHandle = osMessageQueueNew(10, sizeof(ml_result_t), NULL);
  
  // Create all tasks using osThreadNew (CMSIS-RTOS v2 method)
  heartbeatTaskHandle = osThreadNew(heartbeat_monitor_task, NULL, &heartbeat_monitor_attributes);
  mlTaskHandle = osThreadNew(ml_inference_task, NULL, &ml_inference_attributes);
  ledTaskHandle = osThreadNew(led_controller_task, NULL, &led_controller_attributes);
  faultTaskHandle = osThreadNew(fault_handler_task, NULL, &fault_handler_attributes);
  resetTaskHandle = osThreadNew(reset_control_task, NULL, &reset_control_attributes);
  ttcTaskHandle = osThreadNew(ttc_monitor_task, NULL, &ttc_monitor_attributes);
  watchdogTaskHandle = osThreadNew(watchdog_manager_task, NULL, &watchdog_manager_attributes);

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}