/*
 * WorkerTask.h
 *
 *  Created on: Oct 26, 2011
 *      Author: Yuriy
 */

#ifndef WORKERTASK_H_
#define WORKERTASK_H_

#define EVENT_RUN_SPI_TEST 1
#define EVENT_BLINK 2

#include "usart_driver_RTOS.h"
#include "CommandInterpreter.h"
#include "handler.h"
#include "usart_driver_RTOS.h"
#include "spi_driver.h"
#include "led.h"

/** Used to pass arguments to the task */
typedef struct {
	/** Pointer to SpiSlave to use */
	SpiDevice *spiMaster;
	/** Pointer to USART buffer to use. */
	Usart *usart;
	LedGroupEventQueue *led;
	Handler *handler;

} LooperTaskParameters;

void startLooperTask(Handler *handler,
		CommandLineInterpreter *interpreter,
		SpiDevice *master,
		Usart *usart,
		LedGroupEventQueue *led,
		char cPriority, xTaskHandle taskHandle);

#endif /* WORKERTASK_H_ */
