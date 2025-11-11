#ifndef __WS2812B_DRIVER_H
#define __WS2812B_DRIVER_H

#include "main.h"
#include "stm32h7xx_hal.h"

// WS2812B Timing constants (nanoseconds)
#define T0H 350   // 0 code, high time
#define T1H 900   // 1 code, high time  
#define T0L 900   // 0 code, low time
#define T1L 350   // 1 code, low time
#define RESET 50000 // Reset time


#define NS_TO_CYCLES(ns) ((ns * 80) / 1000)  // 80MHz timer

// RGB color structure
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} ws2812b_color_t;

void ws2812b_init(TIM_HandleTypeDef *htim, uint32_t channel);
void ws2812b_set_color(ws2812b_color_t color);
void ws2812b_set_colors(ws2812b_color_t *colors, uint16_t num_leds);
void ws2812b_chase_pattern(ws2812b_color_t color, uint16_t num_cycles);
void ws2812b_breathe_pattern(ws2812b_color_t color, uint16_t duration_ms);

extern const ws2812b_color_t COLOR_NORMAL;
extern const ws2812b_color_t COLOR_ML_ACTIVE;
extern const ws2812b_color_t COLOR_WARNING;
extern const ws2812b_color_t COLOR_DBC_FAULT;
extern const ws2812b_color_t COLOR_TTC_FAULT;
extern const ws2812b_color_t COLOR_CRITICAL;
extern const ws2812b_color_t COLOR_BOOT;
extern const ws2812b_color_t COLOR_CYAN;
extern const ws2812b_color_t COLOR_ORANGE;

#endif