#include "fault_handler.h"
#include "fault_detection.h"
#include "cmsis_os.h"

extern osMessageQueueId_t faultQueueHandle;

void fault_handler_task(void *argument) {
    ml_result_t fault;
    
    for(;;) {
        // Wait for fault messages from the queue
        if (osMessageQueueGet(faultQueueHandle, &fault, NULL, osWaitForever) == osOK) {
            handle_detected_fault(&fault);
        }
    }
}