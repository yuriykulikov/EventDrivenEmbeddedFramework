/*
 * ExmapleHandler.h
 *
 *  Created on: Oct 26, 2011
 *      Author: Yuriy
 */

#ifndef EXAMPLE_HANDLER_H_
#define EXAMPLE_HANDLER_H_

#include "usart_driver_RTOS.h"
#include "usart_driver_RTOS.h"
#include "spi_driver.h"
#include "LedEventProcessorTask.h"
#include "handler.h"

#define EVENT_RUN_SPI_TEST 1
#define EVENT_BLINK 2

/** Used to store context of the handler */
typedef struct {
	/** Pointer to SpiSlave to use */
	SpiDevice *spiMaster;
	/** Pointer to USART buffer to use. */
	Usart *usart;
	LedsEventQueue *led;
} ExampleHandlerContext;

void ExampleHandler_handleMessage(Message msg, void *handlerContext, Handler *handler);

#endif /* EXAMPLE_HANDLER_H_ */
