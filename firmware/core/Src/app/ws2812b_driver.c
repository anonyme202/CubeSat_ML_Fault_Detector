#include "ws2812b_driver.h"
#include "string.h"
#include "math.h"  // Add for sinf function
#include "cmsis_os.h"

static TIM_HandleTypeDef *ws2812b_tim = NULL;
static uint32_t ws2812b_channel = 0;
static uint16_t pwm_buffer[24 * 3 + 100]; // Buffer for 3 LEDs + reset

const ws2812b_color_t COLOR_NORMAL = {0, 255, 0};        // Green
const ws2812b_color_t COLOR_ML_ACTIVE = {0, 0, 255};     // Blue
const ws2812b_color_t COLOR_WARNING = {255, 255, 0};     // Yellow
const ws2812b_color_t COLOR_DBC_FAULT = {255, 0, 0};     // Red
const ws2812b_color_t COLOR_TTC_FAULT = {128, 0, 128};   // Purple
const ws2812b_color_t COLOR_CRITICAL = {255, 0, 0};      // Red
const ws2812b_color_t COLOR_BOOT = {255, 255, 255};      // White
const ws2812b_color_t COLOR_CYAN = {0, 255, 255};        // Cyan
const ws2812b_color_t COLOR_ORANGE = {255, 165, 0};      // Orange


static void color_to_pwm_buffer(ws2812b_color_t color, uint16_t *buffer) {
    uint32_t grb = ((uint32_t)color.green << 16) | 
                   ((uint32_t)color.red << 8) | 
                   color.blue;
    
    for(int i = 0; i < 24; i++) {
        if(grb & (1 << (23 - i))) {
            buffer[i] = NS_TO_CYCLES(T1H); // Bit 1
        } else {
            buffer[i] = NS_TO_CYCLES(T0H); // Bit 0
        }
    }
}

void ws2812b_init(TIM_HandleTypeDef *htim, uint32_t channel) {
    ws2812b_tim = htim;
    ws2812b_channel = channel;
    
    // Initialize buffer with reset condition
    memset(pwm_buffer, 0, sizeof(pwm_buffer));
    
    // Start the timer
    if (ws2812b_tim != NULL) {
        HAL_TIM_PWM_Start(ws2812b_tim, ws2812b_channel);
    }
}

void ws2812b_set_color(ws2812b_color_t color) {
    if (ws2812b_tim == NULL) return;
    
    // Convert color to PWM buffer for first LED, others off
    color_to_pwm_buffer(color, pwm_buffer);
    
    // Set remaining LEDs to off
    ws2812b_color_t off = {0, 0, 0};
    for(int i = 1; i < 3; i++) {
        color_to_pwm_buffer(off, &pwm_buffer[i * 24]);
    }
    
    // Send data
    HAL_TIM_PWM_Start_DMA(ws2812b_tim, ws2812b_channel, (uint32_t*)pwm_buffer, 24 * 3);
    
    // Reset condition
    osDelay(1); // >50us reset
}

void ws2812b_set_colors(ws2812b_color_t *colors, uint16_t num_leds) {
    if (ws2812b_tim == NULL || num_leds == 0) return;
    
    uint16_t buffer_size = num_leds * 24;
    if (buffer_size > sizeof(pwm_buffer)) {
        buffer_size = sizeof(pwm_buffer); // Prevent overflow
    }
    
    uint16_t *buffer = pwm_buffer;
    
    for(int i = 0; i < num_leds && i < 3; i++) { // Max 3 LEDs
        color_to_pwm_buffer(colors[i], buffer);
        buffer += 24;
    }
    
    ws2812b_color_t off = {0, 0, 0};
    for(int i = num_leds; i < 3; i++) {
        color_to_pwm_buffer(off, buffer);
        buffer += 24;
    }
    
    HAL_TIM_PWM_Start_DMA(ws2812b_tim, ws2812b_channel, (uint32_t*)pwm_buffer, 24 * 3);
    osDelay(1); // Reset condition
}

void ws2812b_chase_pattern(ws2812b_color_t color, uint16_t num_cycles) {
    if (ws2812b_tim == NULL) return;
    
    ws2812b_color_t leds[3] = {0}; 
    
    for(int cycle = 0; cycle < num_cycles; cycle++) {
        for(int pos = 0; pos < 3; pos++) {
            // Clear all LEDs
            memset(leds, 0, sizeof(leds));
            // Set current position
            leds[pos] = color;
            
            ws2812b_set_colors(leds, 3);
            osDelay(100);
        }
    }
}

void ws2812b_breathe_pattern(ws2812b_color_t color, uint16_t duration_ms) {
    if (ws2812b_tim == NULL) return;
    
    uint16_t steps = duration_ms / 20;
    if (steps == 0) steps = 1;
    
    for(int i = 0; i < steps; i++) {
        float intensity = (1.0f + sinf(2.0f * 3.14159f * i / steps)) / 2.0f;
        
        ws2812b_color_t breathe_color = {
            .red = (uint8_t)(color.red * intensity),
            .green = (uint8_t)(color.green * intensity),
            .blue = (uint8_t)(color.blue * intensity)
        };
        
        ws2812b_set_color(breathe_color);
        osDelay(20);
    }
}

void ws2812b_set_simple_color(ws2812b_color_t color) {
    if (ws2812b_tim == NULL) return;
    
    // Set all 3 LEDs to the same color
    ws2812b_color_t leds[3] = {color, color, color};
    ws2812b_set_colors(leds, 3);
}