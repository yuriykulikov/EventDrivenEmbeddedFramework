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
#include "CommandInterpreter.h"

/**
 * This struct is used to pass arguments to the task
 */
typedef struct {
	CommandLineInterpreter *interpreter;
	/** Pointer to Usart software abstraction structure to use. */
	Usart *usart;
	size_t commandInputLen;

} CommandInterpreterTaskParameters;

// Prototyping of functions. Documentation is found in source file
void startCommandInterpreterTask (CommandLineInterpreter *interpreter, Usart * usart, char commandInputLen, char priority, xTaskHandle taskHandle);
