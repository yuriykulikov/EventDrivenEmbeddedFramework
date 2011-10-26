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

void CommandInterpreterTask(void *pvParameters) {
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	CommandInterpreterTaskParameters * parameters = (CommandInterpreterTaskParameters *)pvParameters;
	char receivedChar='#';
	char * commandInput = (char *) pvPortMalloc( sizeof(char)*parameters->commandInputLen);

	/* Task loops forever*/
	for (;;) {
		//Empty the string first
		strcpy(commandInput,"");
		// Wait until the first symbol unblocks the task
		Usart_getByte(parameters->usart, &receivedChar,portMAX_DELAY);
		strncat(commandInput,&receivedChar,1);
		//Read string from queue, while data is available and put it into string
		// This loop will be over, when there is either ; or \n is received, or queue is empty for 200 ms
		while (Usart_getByte(parameters->usart, &receivedChar,200)) {
			if (receivedChar == ';') break;
			if (receivedChar == '\n') break;
			strncat(commandInput,&receivedChar,1);
		}

		CommandLineInterpreter_process(parameters->interpreter, (char*)commandInput,parameters->usart);
	}
}

void startCommandInterpreterTask (CommandLineInterpreter *interpreter, Usart *usart, char commandInputLen, char priority, xTaskHandle taskHandle)
{
	CommandInterpreterTaskParameters * parameters = pvPortMalloc(sizeof(CommandInterpreterTaskParameters));
	parameters->interpreter = interpreter;
	parameters->usart = usart;
	parameters->commandInputLen = commandInputLen;
	xTaskCreate(CommandInterpreterTask, (signed char*)"CMDTSK", 300,(void*)parameters, priority, taskHandle);
}

