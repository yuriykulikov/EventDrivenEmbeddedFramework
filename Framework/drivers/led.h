
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

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "avr_compiler.h"

#ifndef LED_H
#define LED_H

#define PORTLED PORTA

#define LED_gm 0x38

//active high
typedef enum {
	RED = 0x20,
	GREEN = 0x10,
    BLUE= 0x08,

    ORANGE = 0x30,
    PINK = 0x28,
    SKY = 0x18,

    WHITE = 0x38,
	NONE = 0x00,
} Color_enum;

typedef struct LEDEventDefenition
{
	uint16_t Duration;
	Color_enum Color;
} LED_event_struct_t;

/* Simple bit mask function, which do not use LED_struct. For debug purpose */
void LED_toggle(Color_enum Color);

/* Simple bit mask function, which do not use LED_struct. For debug purpose */
void LED_set(Color_enum Color);
/* Put led event to the queue */
void LED_queue_put (xQueueHandle queue,Color_enum Color, uint16_t Duration);
xQueueHandle startDebugLedTask (char cPriority);
void BlinkingLedTask( void *pvParameters );
#endif
