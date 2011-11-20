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
    msg.handler = handler;
    msg.what = what;
    xQueueSend(handler->messageQueue, &msg, 2);
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
    msg.handler = handler;
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    xQueueSend(handler->messageQueue, &msg, 2);
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
    msg.handler = handler;
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    msg.ptr = ptr;
    xQueueSend(handler->messageQueue, &msg, 2);
}

/**
 * Creates a handler, which has to be bind to the looper task
 * @param looper
 * @param handleMessage
 * @param context
 */
Handler * Handler_create (Looper *looper, HANDLE_MESSAGE_CALLBACK handleMessage, void *context) {
	// Pack input parameters into the structure and pass them to the task
	Handler *handler = pvPortMalloc(sizeof(Handler));
	handler->messageQueue =  looper->messageQueue;
	handler->handleMessage = handleMessage;
	handler->context = context;
	return handler;
}
