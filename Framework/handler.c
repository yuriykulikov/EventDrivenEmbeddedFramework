/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  Handler
 *
 *      This file contains an utility called handler. It is used to post
 *      messages to be handled by the task immediately or with a delay.
 *      A high-priority task, like command line interpreter should post
 *      messages to the handler.
 *
 * \author
 *      Universitat Erlangen-Nurnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *      Created on: Sep 27, 2011
 *
 *****************************************************************************/


/* Compiler definitions include file. */
#include "avr_compiler.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Task header file */
#include "handler.h"
#include <string.h>
/**
 * Post empty message
 * @param handler
 * @param what
 */
void Handler_sendEmptyMessage(Handler *handler, char what) {
    Message msg;
    msg.what = what;
    xQueueSend(handler->taskQueue, &msg, 2);
}
/**
 * Post message with arguments
 * @param handler
 * @param what
 * @param arg1
 * @param arg2
 */
void Handler_sendMessage(Handler *handler, char what, char arg1, char arg2) {
    Message msg;
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    xQueueSend(handler->taskQueue, &msg, 2);
}
/**
 * Post message with arguments and a pointer to allocated data
 * @param handler
 * @param what
 * @param arg1
 * @param arg2
 * @param ptr
 */
void Handler_sendMessageWithPtr(Handler *handler, char what, char arg1, char arg2, void *ptr) {
    Message msg;
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    msg.ptr = ptr;
    xQueueSend(handler->taskQueue, &msg, 2);
}

/**
 * Creates a handler, which has to be bind to the looper task
 *
 * An example would be:
void HandlerTask( void *pvParameters ) {
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	Handler *handler = (Handler *)pvParameters;
	Message msg;
	for (;;) {
		//block until there is something in the queue
		if (xQueueReceive(handler->taskQueue, &msg, portMAX_DELAY)) {
			switch (msg.what) {
				case EVENT_STUFF:
					handleStuff(msg.arg1, msg.arg2, (Stuff*)msg.ptr);
					break;
				case EVENT_SMTHNG:
					handleSmthing(msg.arg2, (Something*)msg.ptr);
					break;
				default:
					// Oops, unknown message
					break;
			}
		}
	}
}
Handler * startHandlerTask (int stackSize, char priority, xTaskHandle taskHandle,
		short taskQueueSize)
{
	Handler *handler = createHandler(taskHandle);
	xTaskCreate(HandlerTask, (signed char*)"HDLRTASK", stackSize,(void*)handler, priority, taskHandle);
	return handler;
}
 * @param taskQueueSize
 * @return Handler to post messages to
 */
Handler * Handler_create (short taskQueueSize)
{
	// Pack input parameters into the structure and pass them to the task
	Handler *handler = pvPortMalloc(sizeof(Handler));
	handler->taskQueue =  xQueueCreate(taskQueueSize, sizeof(Message));
	return handler;
}
