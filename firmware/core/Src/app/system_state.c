#include "system_state.h"
#include "cmsis_os.h"

// Global system state
system_state_t current_system_state = SYS_STATE_BOOT;

void update_system_state(system_state_t new_state) {
    current_system_state = new_state;
    
    // Update LEDs based on new state
    switch(new_state) {
        case SYS_STATE_BOOT:
            // Chase pattern handled in startup sequence
            break;
        case SYS_STATE_NORMAL:
            // Solid green
            break;
        case SYS_STATE_ML_ACTIVE:
            // Blue blink
            break;
        case SYS_STATE_WARNING:
            // Yellow fast blink
            break;
        case SYS_STATE_OBC_FAULT:
            // Red solid
            break;
        case SYS_STATE_TTC_FAULT:
            // Purple solid
            break;
        case SYS_STATE_CRITICAL:
            // Red fast blink
            break;
        case SYS_STATE_RESET_PENDING:
            // All LEDs blink rapidly
            break;
        default:
            break;
    }
}

system_state_t get_current_system_state(void) {
    return current_system_state;
}