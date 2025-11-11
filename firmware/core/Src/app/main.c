/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "app_x-cube-ai.h"
#include "led_control.h"
#include "reset_control.h" 
#include "fault_detection.h"
#include "ws2812b_driver.h"
#include "ml_integration.h"
#include "ttc_communication.h"
#include "watchdog_manager.h"
#include "system_test.h"

/* Private variables ---------------------------------------------------------*/
reset_control_t system_reset = {0};
system_state_t current_system_state = SYS_STATE_BOOT;
osMessageQueueId_t faultQueueHandle;

// WS2812B Timer handle (replace with your actual timer)
extern TIM_HandleTypeDef htim2;  // Example: using TIM2, change to your timer

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
void MX_FREERTOS_Init(void);

// Add missing task function prototypes
void led_controller_task(void *argument);
void watchdog_manager_task(void *argument);
void fault_handler_task(void *argument);

// Add system initialization function
void system_startup_sequence(void);
void enter_diagnostic_mode(void);
void load_ml_model(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */
  SCB_EnableICache();
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_X_CUBE_AI_Init();

  /* USER CODE BEGIN 2 */
  // Initialize subsystems AFTER peripherals are configured
  led_system_init();
  reset_controller_init();
  watchdog_manager_init();
  
  // Initialize WS2812B driver with timer (replace with your timer and channel)
  ws2812b_init(&htim2, TIM_CHANNEL_1);  // Adjust to your timer configuration
  
  // Run system startup sequence (includes self-test)
  system_startup_sequence();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 22;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// System startup sequence with WS2812B integration
void system_startup_sequence(void) {
    // Boot sequence with LED diagnostics
    ws2812b_chase_pattern(COLOR_BOOT, 3);  // 3 cycles of chase pattern

    // Step 1: MCU initialization
    set_led_blink_rate(LED_SYS_OK, 500);

    // Step 2: Reset circuit initialization
    reset_controller_init();

    // Step 3: Peripheral initialization
    enter_diagnostic_mode();

    // Step 4: ML model loading
    set_led(LED_ML_ACTIVE, 1);
    load_ml_model();
    set_led(LED_ML_ACTIVE, 0);

    // Step 5: System ready - start autonomous monitoring
    set_led(LED_SYS_OK, 1);
    ws2812b_set_simple_color(COLOR_NORMAL);  // Set all RGB LEDs to green
    
    // Update system state
    current_system_state = SYS_STATE_NORMAL;
}

void enter_diagnostic_mode(void) {
    // Flash all LEDs in sequence for diagnostic
    for(int i = 0; i < LED_COUNT; i++) {
        set_led(i, 1);
        osDelay(200);
        set_led(i, 0);
    }
    
    // Test RGB LED with different colors
    ws2812b_set_simple_color(COLOR_RED);
    osDelay(500);
    ws2812b_set_simple_color(COLOR_GREEN);
    osDelay(500);
    ws2812b_set_simple_color(COLOR_BLUE);
    osDelay(500);
    ws2812b_set_simple_color(COLOR_NORMAL);
}

void load_ml_model(void) {
    // Load ML model - implementation depends on STM32Cube.AI
    // This is called by the startup sequence
    osDelay(1000); // Simulate loading time
}

// Add missing task implementations
void led_controller_task(void *argument) {
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // 20Hz update
    
    for(;;) {
        update_leds_task(); // Update LED blinking states
        vTaskDelay(xFrequency);
    }
}

void watchdog_manager_task(void *argument) {
    // Use the corrected watchdog manager task
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(500); // 2Hz update
    
    if (!watchdog_initialized) {
        watchdog_manager_init();
    }
    
    for(;;) {
        // Toggle WDOG_WAKE pin to keep watchdog alive
        HAL_GPIO_TogglePin(WDOG_WAKE_PORT, WDOG_WAKE_PIN);
        last_watchdog_ping = xTaskGetTickCount();
        
        // Check if watchdog has timed out
        if (watchdog_check_timeout()) {
            // Watchdog timeout detected - trigger system reset
            system_reset.watchdog_reset = 1;
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void fault_handler_task(void *argument) {
    ml_result_t fault;
    
    for(;;) {
        // Wait for fault messages from the queue
        if (osMessageQueueGet(faultQueueHandle, &fault, NULL, osWaitForever) == osOK) {
            handle_detected_fault(&fault);
        }
    }
}

// Quick system check function for debugging
void quick_system_check(void) {
    // Test LED subsystem
    for(int i = 0; i < LED_COUNT; i++) {
        set_led(i, 1);
        HAL_Delay(100);
        set_led(i, 0);
    }
    
    // Test RGB LED
    ws2812b_set_simple_color(COLOR_RED);
    HAL_Delay(500);
    ws2812b_set_simple_color(COLOR_GREEN);
    HAL_Delay(500);
    ws2812b_set_simple_color(COLOR_BLUE);
    HAL_Delay(500);
    ws2812b_set_simple_color(COLOR_NORMAL);
    
    // Test reset outputs
    HAL_GPIO_WritePin(ML_FAULT_PORT, ML_FAULT_PIN, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(ML_FAULT_PORT, ML_FAULT_PIN, GPIO_PIN_RESET);
}

/* USER CODE END 4 */

/* MPU Configuration */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  
  // Set all fault LEDs to indicate error state
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_SET);
  
  // Set RGB LED to critical red
  ws2812b_set_simple_color(COLOR_CRITICAL);
  
  while (1)
  {
    // Flash warning LED rapidly
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
    HAL_Delay(100);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */