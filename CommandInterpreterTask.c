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

/* Task header file */
#include "CommandInterpreterTask.h"
#include <string.h>
#include "strings.h"

/* Utils includes. */
#include "CommandInterpreter.h"

// "Fields" of this file
xQueueHandle led;

/**
 * Blinks led
 * @param pcWriteBuffer
 * @param writeBufferLen
 * @return
 */
static portBASE_TYPE blinkLed( signed char *writeBuffer, size_t writeBufferLen ) {
	if(led!=NULL) {
		ledGroupEventQueuePut(led,0x01,100);
		ledGroupEventQueuePut(led,0x03,100);
		ledGroupEventQueuePut(led,0x06,100);
		ledGroupEventQueuePut(led,0x0c,100);
		ledGroupEventQueuePut(led,0x18,100);
		ledGroupEventQueuePut(led,0x30,100);
		ledGroupEventQueuePut(led,0x60,100);
		ledGroupEventQueuePut(led,0x40,100);
		strncpy_P( (char*) writeBuffer, Strings_BlinkResp, writeBufferLen );
	} else {
		strncpy_P( (char*) writeBuffer, Strings_BlinkCmdError, writeBufferLen );
	}
	return pdFALSE;
}

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

	xCmdIntRegisterCommand(Strings_BlinkCmd, Strings_BlinkCmdDesc, blinkLed);

	/* Task loops forever*/
	for (;;)
	{
			//Empty the string first
			strcpy(commandInput,"");
			// Wait until the first symbol unblocks the task
			Usart_getByte(usart, &receivedChar,portMAX_DELAY);
			strncat(commandInput,&receivedChar,1);
			//Read string from queue, while data is available and put it into string
			// This loop will be over, when there is either ; or \n is received, or queue is empty for 200 ms
			while (Usart_getByte(usart, &receivedChar,200))
			{
				if (receivedChar == ';') break;
				if (receivedChar == '\n') break;
				strncat(commandInput,&receivedChar,1);
			}

			for (;;) {
				char pendingCommand = xCmdIntProcessCommand((char*)commandInput,(char*)writerBuffer, writeBufferLen);
				Usart_putString(usart, writerBuffer, 200);
				if (pendingCommand == pdFALSE) break;
			}
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

