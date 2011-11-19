/*
 * BlinkingLedTask.h
 *
 *  Created on: Nov 19, 2011
 *      Author: Yuriy
 */

#ifndef BLINKINGLEDTASK_H_
#define BLINKINGLEDTASK_H_
/* Compiler definitions include file. */
#include "avr_compiler.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/* File headers. */
#include "LedEventProcessorTask.h"

void startBlinkingLedTask (LedsEventQueue * ledGroupEventQueue, char cPriority, xTaskHandle taskHandle);

#endif /* BLINKINGLEDTASK_H_ */
