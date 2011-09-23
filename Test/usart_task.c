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
#include "usart_task.h"
#include <string.h>

/*! \brief Example task
 *
 *  Example task. This task configures USARTD0 for with the parameters:
 *      - 8 bit character size
 *      - No parity
 *      - 1 stop bit
 *      - 9600 Baud
 *
 *  This function then sends three bytes and tests if the received data is
 *  equal to the sent data. The code can be tested by connecting PC3 to PC2. If
 *  the variable 'success' is true at the end of the function, the three bytes
 *  have been successfully sent and received.
*/

void usartTask( void *pvParameters )
{
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	UsartTaskParameters * parameters = (UsartTaskParameters *)pvParameters;
	//store pointer to usart for convenience
	Usart * usartBuffer = parameters->usartBuffer;
	char commandsBufferSize=parameters->commandsBufferSize;
	char receivedChar='#';
	char * str = (char *) pvPortMalloc( sizeof(char)*commandsBufferSize);
	//Declare and initialize exception context
	struct exception_context *the_exception_context;
	init_exception_context(the_exception_context);
	//Declare exception itself
	struct exception e;

	/* Task loops forever*/
	for (;;)
	{
		 Try {
			//Continuously send
			//USART_Buffer_PutString(usart_buffer_t, "g",200);
			//We could have blocked on the queue instead, but this is better for debug
			vTaskDelay(500);

			//Empty the string first
			strcpy(str,"");
			//Read string from queue, while data is available and put it into string
			while (Usart_getByte(usartBuffer, &receivedChar,0))
			{
				strncat(str,&receivedChar,1);
				if (strlen(str)>=commandsBufferSize)
				{
					struct exception e;
					e.type = error;
					e.msg = "Command exceeded buffer size";
					Throw e;
				}
			}
			Usart_putString(usartBuffer, str,200);
			//now check the string for content
			if (strcmp(str,"req_blink")==0)
			{
				if((parameters->debugLed)!=NULL)
				{
					ledGroupEventQueuePut(parameters->debugLed,RED,500);
					ledGroupEventQueuePut(parameters->debugLed,ORANGE,500);
					ledGroupEventQueuePut(parameters->debugLed,PINK,500);
					ledGroupEventQueuePut(parameters->debugLed,WHITE,500);
				}
				Usart_putString(usartBuffer,"resp_blink",200);
			}
			if (strcmp(str,"throw")==0)
			{
				struct exception e;
				e.type = warning;
				e.msg = "demo warning message";
				Throw e;
			}
		} Catch (e) {
			switch (e.type) {
				case warning:
					Usart_putString(usartBuffer, "caught warning:",0);
					Usart_putString(usartBuffer, e.msg,0);
					break;
				case error:
					Usart_putString(usartBuffer, "caught error:",0);
					Usart_putString(usartBuffer, e.msg,0);
					break;
				default:
					Usart_putString(usartBuffer, "caught something else\n",0);
			}//end of switch
		}//end of catch block
	}//end of task's infinite loop
}

void startUsartTask (Usart * usartBuffer, xQueueHandle debugLed, short commandsBufferSize, char cPriority, xTaskHandle taskHandle){
	UsartTaskParameters * usartTaskParameters = pvPortMalloc(sizeof(UsartTaskParameters));
	usartTaskParameters->usartBuffer=usartBuffer;
	usartTaskParameters->debugLed=debugLed;
	usartTaskParameters->commandsBufferSize=commandsBufferSize;
	xTaskCreate(usartTask, (signed char*)"USARTTSK", 1000,(void*)usartTaskParameters, cPriority, taskHandle);
}

