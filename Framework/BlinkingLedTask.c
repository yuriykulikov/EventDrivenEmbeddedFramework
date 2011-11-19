// Scheduler include files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "leds.h"
#include "LedEventProcessorTask.h"

/* Pass pointer to LedGroupEventQueue to vTaskCreate as a parameter */
void BlinkingLedTask( void *pvParameters ) {
	LedsEventQueue * ledGroupEventQueue = (LedsEventQueue *) pvParameters;
	//First blink with SKY, so we see resets
	LedsEvent_put(ledGroupEventQueue,SKY,500);
	vTaskDelay(1000);
	while(true)
	{
		if (xQueueIsQueueEmptyFromISR(ledGroupEventQueue->queueHandle)){
			LedsEvent_put(ledGroupEventQueue,GREEN,500);
		}
		vTaskDelay(1000);
	}
}
/* Starts the blinking led task */
void startBlinkingLedTask (LedsEventQueue * ledGroupEventQueue, char cPriority, xTaskHandle taskHandle){
	xTaskCreate(BlinkingLedTask, ( signed char * ) "BLINK", configMINIMAL_STACK_SIZE, ledGroupEventQueue, configLOW_PRIORITY, taskHandle );
}
