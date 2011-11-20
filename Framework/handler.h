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
#ifndef HANDLER_H_
#define HANDLER_H_
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "Looper.h"

typedef struct MESSAGE Message;
typedef struct HANDLER Handler;
/**
 * The prototype to which handleMessage functions used to process messages must comply.
 * @param msg - message to handle
 * @param *context - context of current Handler. Store variables there
 * @param *handler - pointer to the handler, to modify or look into the queue
 */
typedef void (*HANDLE_MESSAGE_CALLBACK)(Message msg, void *context, Handler *handler);

/** Struct represents handler */
struct HANDLER{
	/** Queue on which handler posts messages */
	xQueueHandle messageQueue;
	/** Function which handles messages*/
	HANDLE_MESSAGE_CALLBACK handleMessage;
	/** Execution context of current handler, handleMessage should cast it to something */
	void *context;
};

/**
 * This structure represents a message
 */
struct MESSAGE{
	/** Handler responsible for handling this message */
	Handler *handler;
	/** What message is about */
	char what;
	/** First argument */
	char arg1;
	/** First argument */
	char arg2;
	/** Pointer to the allocated memory. Handler should cast to the proper type,
	 * according to the message.what */
	void *ptr;
};

/* Prototyping of functions. Documentation is found in source file. */

Handler * Handler_create (Looper *looper, HANDLE_MESSAGE_CALLBACK handleMessage, void *context);
void Handler_sendEmptyMessage(Handler *handler, char what);
void Handler_sendMessage(Handler *handler, char what, char arg1, char arg2);
void Handler_sendMessageWithPtr(Handler *handler, char what, char arg1, char arg2, void *ptr);

#endif /* HANDLER_H_ */
