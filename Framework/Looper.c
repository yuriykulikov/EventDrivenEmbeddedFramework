/*
 * Looper.c
 *
 *  Created on: Nov 20, 2011
 *      Author: Yuriy
 */

#include "handler.h"
#include "Looper.h"

void LooperTask( void *pvParameters )
{
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	Looper *looper = (Looper*)pvParameters;
	//Infinite loop
	for (;;) {
		Message msg;
		if (xQueueReceive(looper->messageQueue, &msg, portMAX_DELAY)) {
			Handler *handler = msg.handler;
			// Call handleMessage from the handler
			handler->handleMessage(msg, handler->context, handler);
		}
	}
}

Looper * Looper_start(unsigned char messageQueueSize, char *name, unsigned short stackDepth, char priority, xTaskHandle taskHandle) {
	Looper *looper = (Looper*) pvPortMalloc(sizeof(Looper));
	looper->messageQueue = xQueueCreate(messageQueueSize, sizeof(Message));
	xTaskCreate(LooperTask, (const signed char * const)name, stackDepth, looper, priority, NULL );
	return looper;
}
