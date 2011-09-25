/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA USART interrupt driven driver example header
 *
 *      This file contains task and function prototypes. 
 *
 *
 * \par Documentation
 *     http://www.FreeRTOS.org - Documentation, latest information.
 *
 *
 * \author
 *      Universität Erlangen-Nürnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 *****************************************************************************/

#include "usart_driver_RTOS.h"

/**
 * This struct is used to pass arguments to the task
 */
typedef struct CommandInterpreterTaskParameters
{
	/** Pointer to Usart software abstraction structure to use. */
	Usart * usart;
	/** Led for debugging. Could be null */
	xQueueHandle led;

	size_t commandInputLen;
	size_t writeBufferLen;
} CommandInterpreterTaskParameters;

// Prototyping of functions. Documentation is found in source file

void startCommandInterpreterTask (char priority, xTaskHandle taskHandle,
		Usart * usart,
		xQueueHandle led,
		size_t commandInputLen,
		size_t writeBufferLen);
