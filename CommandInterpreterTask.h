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
#include "exceptions.h"

/*
 * This struct is used to pass arguments to the task
 */
typedef struct USARTTaskParameters
{
	/* \brief Pointer to USART buffer to use. */
	Usart * usartBuffer;
	/* \brief Led for debugging. Could be null */
	xQueueHandle debugLed;
	/* \brief Command buffer size */
	short commandsBufferSize;
} UsartTaskParameters;


/* Prototyping of functions. Documentation is found in source file. */
void startUsartTask (Usart * usartBuffer, xQueueHandle debugLed, short commandsBufferSize, char cPriority, xTaskHandle taskHandle);
