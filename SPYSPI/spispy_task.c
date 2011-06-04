/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA SPI interrupt-driven driver task source.
 *
 *      This file contains an example task that demonstrates the SPI
 *      interrupt-driven driver.
 *
 * \par Application note:
 *      AVR1309: Using the XMEGA SPI
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
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
 * $Revision: 763 $
 * $Date: 2010-10-10 14:51:56 +0100 (ti, 06 nov 2007) $  \n
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
#include "queue.h"

/* File headers. */
#include "usart_driver_RTOS.h"
#include "led.h"
#include "pmic_driver.h"
#include "spi_driver.h"
#include "spispy_task.h"

/*! \brief Queue handle used to pass date from SPI. */

static xQueueHandle xQueueMOSI;

/*! \brief SPI slave on PORT D. */
SPI_Slave_t spiSlaveD = {NULL, NULL};


void vSPISPYTask( void *pvParameters ){
	/* The parameters are not used. */
	( void ) pvParameters;
	char receivedChar;
	//TODO replace with factory getter
	USART_buffer_struct_t * FTDI_USART = USART_InterruptDriver_Initialize(&USARTC0, BAUD9600, 32);
	for (;;)
	{
		if( xQueueReceive(xQueueMOSI, &receivedChar, MAX_DELAY ) == pdPASS )
		//xQueueReceive(xQueueMOSI, &receivedChar, 0);
		{
			USART_Buffer_PutString(FTDI_USART,"MOSI:0x",DONT_BLOCK);
			USART_Buffer_PutInt(FTDI_USART,receivedChar,16,DONT_BLOCK);
		}
	}
}

/*! \brief UASART Task launcher
 *
 *  Starts the USART task with specified priority.
 */

void vStartSPISPYTask(char cPriority)
{
	/* Create a queue.
	 * On XMEGA port create all queues before vStartTaskScheduler() to ensure that heap size is enough */
	xQueueMOSI = xQueueCreate( 16, sizeof(char));
	/* Initialize SPI slave on port D. */
	SPI_SlaveInit(&spiSlaveD,&SPID,&PORTD,false,SPI_MODE_0_gc,SPI_INTLVL_MED_gc);
	/* Enable low and medium level interrupts in the interrupt controller. */
	PMIC_EnableLowLevel();
	PMIC_EnableMediumLevel();
	/* Spawn task */
	xTaskCreate(vSPISPYTask, ( signed char * ) "SPISPY", configMINIMAL_STACK_SIZE+100, NULL, cPriority, NULL );
}

/*! \brief SPI slave interrupt service routine.
 *
 *  If extensive computation is needed, it is recommended
 *  to do this in a function, which is then called by the ISR.
 *
 *  Similar ISRs must be added if other SPI modules are to be used.
 */

ISR(SPID_INT_vect)
{
	/* Local variables. */
	uint8_t receivedChar;
	signed char xHigherPriorityTaskWoken = pdFALSE;
	/* Get received data. */
	receivedChar = SPI_SlaveReadByte(&spiSlaveD);
	/* Send back something all ones so it doesn't have any effect. */
	SPI_SlaveWriteByte(&spiSlaveD, 0xff);
	/* Post the character on the queue of received characters.*/
	xQueueSendToBackFromISR( xQueueMOSI, &receivedChar, &xHigherPriorityTaskWoken );
	/* If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */
	if( xHigherPriorityTaskWoken ) taskYIELD();
}


