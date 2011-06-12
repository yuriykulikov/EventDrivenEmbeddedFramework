
/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA board header
 *
 *      This file contains board defenitions and board-specific task and function prototypes. 
 *
 *
 * \par Documentation
 *     http://www.FreeRTOS.org - Documentation, latest information.
 *
 * \par Application note:
 *      AVR1000: Getting Started Writing C-code for XMEGA
 *
 * \author
 *      Universität Erlangen-Nürnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 *****************************************************************************/
#ifndef LED_H
#define LED_H
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "avr_compiler.h"

#include "ledGroup.h"


/* Led event, described by bitmask and duration */
typedef struct LEDEventDefenition
{
	uint16_t duration;
	uint8_t bitmask;
} LedGroupEvent;
/* A queue to put and then process Led events */
typedef struct LedGroupEventQueueDefenition
{
	LedGroup * ledGroup;
	xQueueHandle queueHandle;
} LedGroupEventQueue;

//---------------------------------------

void ledGroupEventQueuePut (LedGroupEventQueue * ledGroupEventQueue,uint8_t bitmask, uint16_t duration);
LedGroupEventQueue * startLedQueueProcessorTask (LedGroup * ledGroup, char cPriority, xTaskHandle taskHandle);
xTaskHandle startBlinkingLedTask (LedGroupEventQueue * ledGroupEventQueue, char cPriority);
#endif
