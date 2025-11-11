#ifndef __SYSTEM_STATE_H
#define __SYSTEM_STATE_H

#include "main.h"

// System states (from firmware specification)
typedef enum {
    SYS_STATE_BOOT = 0,           // System initialization
    SYS_STATE_NORMAL,             // Normal operation
    SYS_STATE_ML_ACTIVE,          // Machine learning inference active
    SYS_STATE_DATA_COLLECTION,    // Telemetry data collection
    SYS_STATE_WARNING,            // Minor anomaly detected
    SYS_STATE_OBC_DEGRADATION,    // OBC performance degradation
    SYS_STATE_OBC_FAULT,          // OBC fault detected
    SYS_STATE_TTC_FAULT,          // TTC fault detected
    SYS_STATE_CRITICAL,           // Critical system fault
    SYS_STATE_DEMO,               // Demonstration mode
    SYS_STATE_RESET_PENDING       // Reset sequence active
} system_state_t;

// Function prototypes
void update_system_state(system_state_t new_state);
system_state_t get_current_system_state(void);

#endif
