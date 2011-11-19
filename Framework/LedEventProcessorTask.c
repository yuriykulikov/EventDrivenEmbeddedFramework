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
#include "LedEventProcessorTask.h"
#include "leds.h"

void LedsEvent_put (LedsEventQueue * ledGroupEventQueue,uint8_t bitmask, uint16_t duration)
{
	//Create new empty LED structure
	//This is a local variable, it will exist only until the function return.
	LedsEvent newLedEvent;
	//Set value according to parameters
	newLedEvent.bitmask=bitmask;
	newLedEvent.duration=duration;
	/* Send  event data to queue. After this LEDTask will be unblocked as soon as
	 * all higher priority tasks get blocked, delayed, suspended - whatever.*/
	xQueueSendToBack(ledGroupEventQueue->queueHandle,&newLedEvent,0);
}

void ledQueueProcessorTask( void *pvParameters )
{
	LedsEventQueue * ledGroupEventQueue = (LedsEventQueue *) pvParameters;
	LedsEvent newLedEvent;
	/* Cycle for ever*/
	while(true)
	{
		//Check while there are new events on the queue. Block if nothing.
		while(xQueueReceive(ledGroupEventQueue->queueHandle, &newLedEvent, portMAX_DELAY ) == pdPASS )
		{
			Leds_set(ledGroupEventQueue->ledGroup, newLedEvent.bitmask);
			vTaskDelay(newLedEvent.duration);
			Leds_set(ledGroupEventQueue->ledGroup, NONE);
		}
	}
}

/* Creates a queue and spawns a task */
LedsEventQueue * LedsEvent_startLedsTask (LedGroup * ledGroup, char cPriority, xTaskHandle taskHandle)
{
	/* Create Queue for LED events */
	LedsEventQueue * ledGroupEventQueue = pvPortMalloc(sizeof(LedsEventQueue));
	ledGroupEventQueue->queueHandle=xQueueCreate(8, sizeof(LedsEvent));
	ledGroupEventQueue->ledGroup=ledGroup;
	/* Spawn task. */
	xTaskCreate(ledQueueProcessorTask, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, ledGroupEventQueue, cPriority, taskHandle );
	return ledGroupEventQueue;
};
