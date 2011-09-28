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

/**
 * This structure represents a message
 */
typedef struct
{
	/** What message is about */
	char what;
	/** First argument */
	char arg1;
	/** First argument */
	char arg2;
	/** Pointer to the allocated memory. Handler should cast to the proper type,
	 * according to the message.what */
	void *ptr;
} Message;

/** Struct represents handler */
typedef struct
{
	/** Queue on which looper task should block */
	xQueueHandle taskQueue;
} Handler;

/* Prototyping of functions. Documentation is found in source file. */
Handler * Handler_create (short taskQueueSize);

void Handler_sendEmptyMessage(Handler *handler, char what);
void Handler_sendMessage(Handler *handler, char what, char arg1, char arg2);
void Handler_sendMessageWithPtr(Handler *handler, char what, char arg1, char arg2, void *ptr);

#endif /* HANDLER_H_ */
