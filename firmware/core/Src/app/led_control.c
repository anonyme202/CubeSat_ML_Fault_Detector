#include "led_control.h"
#include "cmsis_os.h"

// LED control array - EXACT MATCH with hardware design
led_control_t system_leds[LED_COUNT] = {
    {GPIOC, GPIO_PIN_0, 1000, 0, 0, 0, "ML_ACTIVE"},      // PC0
    {GPIOC, GPIO_PIN_1, 0, 0, 0, 0, "FAULT_DBC"},         // PC1
    {GPIOC, GPIO_PIN_2, 0, 0, 0, 0, "FAULT_TTC"},         // PC2  
    {GPIOC, GPIO_PIN_3, 500, 0, 0, 0, "WARNING"},         // PC3
    {GPIOC, GPIO_PIN_4, 1000, 0, 0, 0, "HEARTBEAT"},      // PC4
    {GPIOC, GPIO_PIN_5, 200, 0, 0, 0, "COMM_ACTIVE"},     // PC5
    {GPIOC, GPIO_PIN_9, 0, 0, 0, 0, "SYS_OK"}             // PC9
};

// Pre-defined colors for system states
const rgb_color_t COLOR_NORMAL = {0, 255, 0};        // Green
const rgb_color_t COLOR_ML_ACTIVE = {0, 0, 255};     // Blue
const rgb_color_t COLOR_WARNING = {255, 255, 0};     // Yellow
const rgb_color_t COLOR_DBC_FAULT = {255, 0, 0};     // Red
const rgb_color_t COLOR_TTC_FAULT = {128, 0, 128};   // Purple
const rgb_color_t COLOR_CRITICAL = {255, 0, 0};      // Red
const rgb_color_t COLOR_BOOT = {255, 255, 255};      // White
const rgb_color_t COLOR_CYAN = {0, 255, 255};        // Cyan
const rgb_color_t COLOR_ORANGE = {255, 165, 0};      // Orange

void led_system_init(void) {
    // LEDs are initialized by CubeMX GPIO init
    // Turn all LEDs off initially
    for(int i = 0; i < LED_COUNT; i++) {
        HAL_GPIO_WritePin(system_leds[i].port, system_leds[i].pin, GPIO_PIN_RESET);
    }
}

void set_led(led_id_t led_id, uint8_t state) {
    if(led_id < LED_COUNT) {
        system_leds[led_id].current_state = state;
        HAL_GPIO_WritePin(system_leds[led_id].port, system_leds[led_id].pin, 
                         state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

void set_led_blink_rate(led_id_t led_id, uint32_t interval_ms) {
    if(led_id < LED_COUNT) {
        system_leds[led_id].blink_interval_ms = interval_ms;
        system_leds[led_id].last_toggle_time = osKernelGetTickCount();
    }
}

void update_leds_task(void) {
    uint32_t current_time = osKernelGetTickCount();
    
    for(int i = 0; i < LED_COUNT; i++) {
        if(system_leds[i].blink_interval_ms > 0) {
            if((current_time - system_leds[i].last_toggle_time) >= 
               system_leds[i].blink_interval_ms) {
                // Toggle LED state
                system_leds[i].current_state = !system_leds[i].current_state;
                HAL_GPIO_WritePin(system_leds[i].port, system_leds[i].pin,
                                 system_leds[i].current_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
                system_leds[i].last_toggle_time = current_time;
            }
        }
    }
}

void update_leds_from_ml_result(ml_result_t* result) {
    // Reset all fault indicators
    set_led(LED_FAULT_DBC, 0);
    set_led(LED_FAULT_TTC, 0);
    set_led(LED_WARNING, 0);
    
    switch(result->predicted_class) {
        case 0: // Normal
            set_led(LED_SYS_OK, 1);
            ws2812b_set_color(COLOR_NORMAL);
            break;
        case 1: // OBC Fault (No heartbeat)
            set_led(LED_FAULT_DBC, 1);
            set_led(LED_SYS_OK, 0);
            ws2812b_set_color(COLOR_DBC_FAULT);
            break;
        case 2: // Overcurrent
            set_led(LED_WARNING, 1);
            set_led_blink_rate(LED_WARNING, 250);
            ws2812b_set_color(COLOR_WARNING);
            break;
        case 3: // TTC Fault
            set_led(LED_FAULT_TTC, 1);
            set_led(LED_SYS_OK, 0);
            ws2812b_set_color(COLOR_TTC_FAULT);
            break;
        case 4: // Data corruption
            set_led(LED_WARNING, 1);
            set_led_blink_rate(LED_WARNING, 100);
            ws2812b_set_color(COLOR_ORANGE);
            break;
    }
}
void led_controller_task(void *argument) {
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // 20Hz update
    
    for(;;) {
        update_leds_task(); // Update LED blinking states
        vTaskDelay(xFrequency);
    }
}

// WS2812B Driver (simplified - you'll need to implement timing-specific code)
void ws2812b_set_color(rgb_color_t color) {
    // Implementation depends on your specific WS2812B connection
    // This is a placeholder - you'll need to implement the precise timing
}

void ws2812b_chase_pattern(rgb_color_t color) {
    // Implement chase pattern for boot sequence
}