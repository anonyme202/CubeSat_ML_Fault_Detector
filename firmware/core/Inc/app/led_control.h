#ifndef __LED_CONTROL_H
#define __LED_CONTROL_H

#include "main.h"

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t blink_interval_ms;
    uint32_t last_toggle_time;
    uint8_t current_state;
    uint8_t default_state;
    char* name;
} led_control_t;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color_t;

void led_system_init(void);
void set_led(led_id_t led_id, uint8_t state);
void set_led_blink_rate(led_id_t led_id, uint32_t interval_ms);
void update_leds_task(void);
void update_leds_from_ml_result(ml_result_t* result);
void ws2812b_set_color(rgb_color_t color);
void ws2812b_chase_pattern(rgb_color_t color);

#endif