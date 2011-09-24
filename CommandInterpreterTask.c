/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA USART interrupt driven driver example source.
 *
 *      This file contains an example task that demonstrates the
 *      interrupt driven USART driver. The task continiously sends 'L' bytes and
 *		listens to the port. If any data is recieved it sends it back. 
 *
 * \par Application note:
 *      AVR1307: Using the XMEGA USART
 *
 * \par Documentation
 *     http://www.FreeRTOS.org - Documentation, latest information.
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * \author
 *      Universität Erlangen-Nürnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/


/* Compiler definitions include file. */
#include "avr_compiler.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* File headers. */
#include "led.h"
#include "usart_driver_RTOS.h"

#include "exceptions.h"

/* Task header file */
#include "CommandInterpreterTask.h"
#include <string.h>

/* Utils includes. */
#include "CommandInterpreter.h"

// "Fields" of this file
//struct exception e;
//struct exception_context *the_exception_context;
xQueueHandle led;
/**
 * Blinks led
 * @param pcWriteBuffer
 * @param writeBufferLen
 * @return
 */
static portBASE_TYPE blinkLed( signed char *writeBuffer, size_t writeBufferLen ) {
	if(led!=NULL) {
		ledGroupEventQueuePut(led,RED,500);
		ledGroupEventQueuePut(led,ORANGE,500);
		ledGroupEventQueuePut(led,PINK,500);
		ledGroupEventQueuePut(led,WHITE,500);
		strncpy( (char*) writeBuffer, "resp_blink ok\r\n", writeBufferLen );
	} else {
		strncpy( (char*) writeBuffer, "resp_blink error: no led assigned\r\n", writeBufferLen );
	}
	return pdFALSE;
}
/** The definition of the "blink" command.*/
static const xCommandLineInput blinkCommand = {
	( const signed char * const ) "req_blink",
	( const signed char * const ) "req_blink: Blinks the LED\r\n",
	blinkLed
};

/**
 * Throws an exception
 * @param pcWriteBuffer
 * @param writeBufferLen
 * @return
 */
static portBASE_TYPE throw( signed char *writeBuffer, size_t writeBufferLen ) {
//	e.type = warning;
//	e.msg = "demo warning message";
//	Throw e;
	return pdFALSE;
}
/** The definition of the "blink" command.*/
static const xCommandLineInput throwCommand = {
	( const signed char * const ) "throw",
	( const signed char * const ) "throw: Throws an exception\r\n",
	throw
};

void CommandInterpreterTask( void *pvParameters ) {
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	CommandInterpreterTaskParameters * parameters = (CommandInterpreterTaskParameters *)pvParameters;
	// Extract parameters
	Usart * usart = parameters->usart;
	led = parameters->led;
	size_t commandInputLen = parameters->commandInputLen;
	size_t writeBufferLen = parameters->writeBufferLen;

	char receivedChar='#';
	char * commandInput = (char *) pvPortMalloc( sizeof(char)*commandInputLen);
	char * writerBuffer = (char *) pvPortMalloc( sizeof(char)*writeBufferLen);
	//initialize exception context
//	init_exception_context(the_exception_context);

	xCmdIntRegisterCommand(&blinkCommand);
	xCmdIntRegisterCommand(&throwCommand);

	/* Task loops forever*/
	for (;;)
	{
//		 Try {

			//Empty the string first
			strcpy(commandInput,"");
			//Read string from queue, while data is available and put it into string
			while (Usart_getByte(usart, &receivedChar,portMAX_DELAY))
			{
				if (receivedChar == ';') break;
				if (receivedChar == '\n') break;
				strncat(commandInput,&receivedChar,1);
				if (strlen(commandInput)>=commandInputLen)
				{
//					e.type = error;
//					e.msg = "Command exceeded buffer size";
//					Throw e;
				}
			}

			for (;;) {
				char pendingCommand = xCmdIntProcessCommand((signed char*)commandInput,(signed char*)writerBuffer, writeBufferLen);
				Usart_putString(usart, writerBuffer, 200);
				if (pendingCommand == pdFALSE) break;
			}

//		} Catch (e) {
//			switch (e.type) {
//				case warning:
//					Usart_putString(usartBuffer, "caught warning:",0);
//					Usart_putString(usartBuffer, e.msg,0);
//					break;
//				case error:
//					Usart_putString(usartBuffer, "caught error:",0);
//					Usart_putString(usartBuffer, e.msg,0);
//					break;
//				default:
//					Usart_putString(usartBuffer, "caught something else\n",0);
//			}//end of switch
//		}//end of catch block
	}//end of task's infinite loop
}

void startCommandInterpreterTask (char priority, xTaskHandle taskHandle,
		Usart * usart,
		xQueueHandle led,
		size_t commandInputLen,
		size_t writeBufferLen)
{
	CommandInterpreterTaskParameters * parameters = pvPortMalloc(sizeof(CommandInterpreterTaskParameters));
	parameters->usart = usart;
	parameters->led = led;
	parameters->commandInputLen = commandInputLen;
	parameters->writeBufferLen = writeBufferLen;
	xTaskCreate(CommandInterpreterTask, (signed char*)"CMDTSK", 1000,(void*)parameters, priority, taskHandle);
}

