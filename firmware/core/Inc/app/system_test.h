#ifndef __SYSTEM_TEST_H
#define __SYSTEM_TEST_H

#include "main.h"

void run_system_self_test(void);
void test_led_system(void);
void test_reset_system(void);
void test_communication_system(void);
void test_ml_system(void);
void test_fault_detection(void);

#endif