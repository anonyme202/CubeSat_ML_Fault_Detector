#ifndef __ML_INTEGRATION_H
#define __ML_INTEGRATION_H

#include "main.h"
#include "ai_runtime.h"  // STM32Cube.AI generated header

#define ML_INPUT_SIZE 32   // Adjust based on your model
#define ML_OUTPUT_SIZE 5   // 5 fault classes

typedef struct {
    float input_buffer[ML_INPUT_SIZE];
    float output_buffer[ML_OUTPUT_SIZE];
    ai_handle network;
    ai_buffer* ai_input;
    ai_buffer* ai_output;
} ml_model_t;

// ML function prototypes
uint8_t ml_model_init(ml_model_t *model);
uint8_t ml_model_run_inference(ml_model_t *model, float *input_data, float *output_data);
void ml_model_deinit(ml_model_t *model);
void collect_ml_input_data(float *input);
ml_result_t process_ml_output(float *output);

#endif