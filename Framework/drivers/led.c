/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA board-specific functions and tasks source.
 *
 *      This file contains functions to use led events. Led event is a bitmask to be lit,
 *      and time for which it should stay.
 *     
 * \author
 *      Universitaet Erlangen-Nuernberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 * \par Documentation
 *     http://www.FreeRTOS.org - Documentation, latest information.
 *
 *****************************************************************************/

/* Compiler definitions include file. */
#include "avr_compiler.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/* File headers. */
#include "led.h"
#include "ledGroup.h"

void ledGroupEventQueuePut (LedGroupEventQueue * ledGroupEventQueue,uint8_t bitmask, uint16_t duration)
{
	//Create new empty LED structure
	//This is a local variable, it will exist only until the function return.
	LedGroupEvent newLedEvent;
	//Set value according to parameters
	newLedEvent.bitmask=bitmask;
	newLedEvent.duration=duration;
	/* Send  event data to queue. After this LEDTask will be unblocked as soon as
	 * all higher priority tasks get blocked, delayed, suspended - whatever.*/
	xQueueSendToBack(ledGroupEventQueue->queueHandle,&newLedEvent,0);
}
//TODO use timer.c to control LED
void ledQueueProcessorTask( void *pvParameters )
{
	LedGroupEventQueue * ledGroupEventQueue = (LedGroupEventQueue *) pvParameters;
	LedGroupEvent newLedEvent;
	/* Cycle for ever*/
	while(true)
	{
		//Check while there are new events on the queue. Block if nothing.
		while(xQueueReceive(ledGroupEventQueue->queueHandle, &newLedEvent, portMAX_DELAY ) == pdPASS )
		{
			ledGroupSet(ledGroupEventQueue->ledGroup, newLedEvent.bitmask);
			vTaskDelay(newLedEvent.duration);
			ledGroupSet(ledGroupEventQueue->ledGroup, NONE);
		}
	}
}

/* Creates a queue and spawns a task */
LedGroupEventQueue * startLedQueueProcessorTask (LedGroup * ledGroup, char cPriority, xTaskHandle taskHandle)
{
	/* Create Queue for LED events */
	LedGroupEventQueue * ledGroupEventQueue = pvPortMalloc(sizeof(LedGroupEventQueue));
	ledGroupEventQueue->queueHandle=xQueueCreate(8, sizeof(LedGroupEvent));
	ledGroupEventQueue->ledGroup=ledGroup;
	/* Spawn task. */
	xTaskCreate(ledQueueProcessorTask, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, ledGroupEventQueue, cPriority, taskHandle );
	return ledGroupEventQueue;
};

/* Pass pointer to LedGroupEventQueue to vTaskCreate as a parameter */
void BlinkingLedTask( void *pvParameters )
{
	LedGroupEventQueue * ledGroupEventQueue = (LedGroupEventQueue *) pvParameters;
	//First blink with SKY, so we see resets
	ledGroupEventQueuePut(ledGroupEventQueue,SKY,500);
	vTaskDelay(1000);
	while(true)
	{
		if (xQueueIsQueueEmptyFromISR(ledGroupEventQueue->queueHandle)){
			ledGroupEventQueuePut(ledGroupEventQueue,GREEN,500);
		}
		vTaskDelay(1000);
	}
}
/* Starts the blinking led task */
void startBlinkingLedTask (LedGroupEventQueue * ledGroupEventQueue, char cPriority, xTaskHandle taskHandle){
	xTaskCreate(BlinkingLedTask, ( signed char * ) "BLINK", configMINIMAL_STACK_SIZE, ledGroupEventQueue, configLOW_PRIORITY, taskHandle );
}
