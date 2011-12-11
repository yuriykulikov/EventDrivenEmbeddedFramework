/* This file has been prepared for Doxygen automatic documentation generation.*/
/** \file *********************************************************************
 *
 * @brief
 *      XMEGA USART FreeRTOS driver source file.
 *
 *      This file contains the function implementations of the XMEGA interrupt
 *      FreeRTOS-based driver.
 *
 * \par Application note:
 *      AVR1307: Using the XMEGA USART
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *  \author
 *   	Universität Erlangen-Nürnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 * $Revision: 1694 $
 * $Date: 2008-07-29 14:21:58 +0200 (ti, 29 jul 2008) $  \n
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

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "port_driver.h"
#include "usart.h"
#include "usart_driver_RTOS.h"
#include <avr/pgmspace.h>
//Structures, representing uart and its buffer. for internal use.
//Memory allocated dynamically
Usart * usartC;
Usart * usartD;
Usart * usartE;

signed char USART_RXComplete(Usart *);
signed char USART_DataRegEmpty(Usart *);

ISR(USARTC0_RXC_vect){ if( USART_RXComplete(usartC) ) taskYIELD(); }
ISR(USARTD0_RXC_vect){ if( USART_RXComplete(usartD) ) taskYIELD(); }
ISR(USARTE0_RXC_vect){ if( USART_RXComplete(usartE) ) taskYIELD(); }

ISR(USARTC0_DRE_vect){USART_DataRegEmpty(usartC);}
ISR(USARTD0_DRE_vect){USART_DataRegEmpty(usartD);}
ISR(USARTE0_DRE_vect){USART_DataRegEmpty(usartE);}

/**
 * @brief Initializes buffer and selects what USART module to use.
 *
 * This function is a "constructor", it allocates memory,
 * initializes receive and transmit buffer and selects what USART module to use,
 * and stores the data register empty interrupt level.
 * @param module hardware module to use
 * @param baudrate
 * @param bufferSize
 * @param ticksToWait - Default wait time
 * @return pointer to the Usart software module
 */
Usart * Usart_initialize(USART_t *module, Baudrate baudrate, char bufferSize, int ticksToWait) {
	//We use only low level interrupts, but leave local variable in case we change the mind
	USART_DREINTLVL_t dreIntLevel = USART_DREINTLVL_LO_gc;
	PORT_t * port;
	Usart * usart;
	//Allocate memory for usart structure and store the pointer
	usart = ( Usart * ) pvPortMalloc( sizeof( Usart ) );
	//switch by pointer to usart structure, which will be used. Usually it is not a good idea to
	//switch by pointer, but in our case pointers are defined by hardware
	switch ((int)module) {
		case (int)&USARTC0:
			//copy pointer pUsartBuffer to global pUsartBufferC, which is use to handle interrupts
			usartC = usart;
			//Since usart is on the port C, we will need to use PORTC
			port = &PORTC;
			break;
		case (int)&USARTD0:
			//copy pointer pUsartBuffer to global pUsartBufferD, which is use to handle interrupts
			usartD = usart;
			//Since usart is on the port D, we will need to use PORTD
			port = &PORTD;
			break;
		case (int)&USARTE0:
			//copy pointer pUsartBuffer to global pUsartBufferD, which is use to handle interrupts
			usartE = usart;
			//Since usart is on the port E, we will need to use PORTE
			port = &PORTE;
			break;
		default:
			//use default, TODO: report error here with LED
			break;
	}

	/* (TX0) as output. */
	port->DIRSET = PIN3_bm;
	/* (RX0) as input. */
	port->DIRCLR = PIN2_bm;
	//totempole and pullup
	PORT_ConfigurePins( port,PIN3_bm,false,false,PORT_OPC_PULLUP_gc,PORT_ISC_BOTHEDGES_gc );
	/* Initialize buffers. Create a queue (allocate memory) and store queue handle in Usart
	 * On XMEGA port create all queues before vStartTaskScheduler() to ensure that heap size is enough */
	/* Store pointer to USART module */
	usart->module = module;
	/*Store DRE level so we will know which level to enable when we put data and want it to be sent. */
	usart->dreIntLevel = dreIntLevel;
	// store default ticksToWait value - used in Dflt functions
	usart->ticksToWait = ticksToWait;
	/* @brief  Receive buffer size: 2,4,8,16,32,64,128 bytes. */
	usart->RXqueue = xQueueCreate(bufferSize,sizeof(char));
	usart->TXqueue = xQueueCreate(bufferSize,sizeof(char));

	/* USARTD0, 8 Data bits, No Parity, 1 Stop bit. */
	USART_Format_Set(usart->module, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
	/* Enable RXC interrupt. */
	USART_RxdInterruptLevel_Set(usart->module, USART_RXCINTLVL_LO_gc);

	//http://prototalk.net/forums/showthread.php?t=188
	switch (baudrate) {
		case BAUD9600:
			USART_Baudrate_Set(usart->module, 3317 , -4);
			break;
		default:
			//9600
			USART_Baudrate_Set(usart->module, 3317 , -4);
			break;
	}

	/* Enable both RX and TX. */
	USART_Rx_Enable(usart->module);
	USART_Tx_Enable(usart->module);
	//return the software module structure, to be used for reading and writing
	return usart;
}

/** @brief Put data (5-8 bit character).
 *
 *  Stores data byte in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 * @param usart
 * @param data The data to send
 * @param ticksToWait Amount of RTOS ticks (1 ms default) to wait if there is space in queue
 * @return pdTRUE is success, pdFALSE if queue was full and ticksToWait elapsed
 */
int8_t Usart_putByte(Usart * usart, uint8_t data, int ticksToWait )
{
	uint8_t tempCTRLA;
	signed char queueSendResult = xQueueSendToBack(usart->TXqueue, &data, ticksToWait);
	/* If we successfully loaded byte to queue */
	if (queueSendResult == pdPASS) {
		/* Enable DRE interrupt. */
		tempCTRLA = usart->module->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | usart->dreIntLevel;
		usart->module->CTRLA = tempCTRLA;
		return pdPASS;
	} else {
		return pdFAIL;
	}
}

/** @brief Get received data
 *
 *  Returns pdTRUE is data is available and puts byte into &receivedChar variable
 *
 *  @param usart_struct       The USART_struct_t struct instance.
 *	@param receivedChar       Pointer to char variable for to save result.
 *	@param xTicksToWait       Amount of RTOS ticks (1 ms default) to wait if there is data in queue.
 *  @return					  Success.
 */
inline int8_t Usart_getByte(Usart * usart, char * receivedChar, int ticksToWait )
{
	return xQueueReceive(usart->RXqueue, receivedChar, ticksToWait);
}
/** @brief Send string via Usart
 *
 *  Stores data string in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 *
 *  @param usart_struct The USART_struct_t struct instance.
 *  @param string       The string to send.
 *  @param xTicksToWait       Amount of RTOS ticks (1 ms default) to wait if there is space in queue.
 */
inline int8_t Usart_putString(Usart * usart, const char *string, int ticksToWait )
{
	//send the whole string. Note that if buffer is full, USART_TXBuffer_PutByte will do nothing
	while (*string) {
		int8_t putByteResult = Usart_putByte(usart, *string++, ticksToWait);
		if (putByteResult == pdFAIL) return pdFAIL;
	}
	return pdPASS;
}
/** @brief Send program memory string via Usart
 *
 *  Stores data string in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 *  String is taken from the program memory.
 *
 *  @param usart_struct The USART_struct_t struct instance.
 *  @param string       The string to send.
 *  @param xTicksToWait       Amount of RTOS ticks (1 ms default) to wait if there is space in queue.
 */
inline int8_t Usart_putPgmString(Usart * usart, const char *progmem_s, int ticksToWait )
{
	register char c;
	while ( (c = pgm_read_byte(progmem_s++)) ) {
		int8_t putByteResult =  Usart_putByte(usart, c, ticksToWait);
		if (putByteResult == pdFAIL) return pdFAIL;
	}
	return pdPASS;
}
/** @brief Put data (5-8 bit character).
 *
 *  Stores data integer represented as string in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 *
 *  @param usart Usart software abstraction structure
 *  @param Int       The integer to send.
 *  @param radix	Integer basis - 10 for decimal, 16 for hex
 *  @param xTicksToWait
 */
int8_t Usart_putInt(Usart * usart, int16_t Int,int16_t radix, int ticksToWait )
{
	char * str="big string for some itoa uses";
	return Usart_putString(usart, itoa(Int,str,radix), ticksToWait );
}

/** @brief Put data
 *  Stores data byte in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 * @param usart
 * @param data The data to send
 * @return pdTRUE is success, pdFALSE if queue was full and ticksToWait elapsed
 */
inline int8_t Usart_putByteDflt(Usart * usart, uint8_t data) {
	return Usart_putByte(usart, data, usart->ticksToWait);
}
/** @brief Send string via Usart
 *  Stores data string in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 *  @param usart_struct The USART_struct_t struct instance.
 *  @param string       The string to send.
 */
inline int8_t Usart_putStringDflt(Usart * usart, const char *string) {
	return Usart_putString(usart, string, usart->ticksToWait);
}
/** @brief Send program memory string via Usart
 *  Stores data string in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 *  String is taken from the program memory.
 *  @param usart_struct The USART_struct_t struct instance.
 *  @param string       The string to send.
 */
inline int8_t Usart_putPgmStringDflt(Usart * usart, const char *progmem_s) {
	return Usart_putPgmString(usart, progmem_s, usart->ticksToWait);
}
/** @brief Put data (5-8 bit character).
 *  Stores data integer represented as string in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 *  @param usart Usart software abstraction structure
 *  @param Int       The integer to send.
 *  @param radix	Integer basis - 10 for decimal, 16 for hex
 */
inline int8_t Usart_putIntDflt(Usart * usart, int16_t Int,int16_t radix) {
	return Usart_putInt(usart, Int, radix, usart->ticksToWait);
}
/** @brief Get received data
 *
 *  Returns pdTRUE is data is available and puts byte into &receivedChar variable
 *
 *  @param usart_struct       The USART_struct_t struct instance.
 *	@param receivedChar       Pointer to char variable for to save result.
 *  @return					  Success.
 */
inline int8_t Usart_getByteDflt(Usart * usart, char * receivedChar) {
	return Usart_getByte(usart, receivedChar, usart->ticksToWait);
}

/**
 * Receive complete interrupt service routine
 *
 * Stores received data in RX software buffer.
 * @param Usart software abstraction structure
 * @return xHigherPriorityTaskWoken
 */
inline signed char USART_RXComplete(Usart * usart)
{
	/* We have to check is we have woke higher priority task, because we post to
	 * queue and high priority task might be blocked waiting for items appear on
	 * this queue */
	signed char xHigherPriorityTaskWoken = pdFALSE;
	signed char cChar;
	/* Get the character and post it on the queue of Rxed characters.
	If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */
	cChar = usart->module->DATA;
	xQueueSendToBackFromISR( usart->RXqueue, &cChar, &xHigherPriorityTaskWoken );
	return xHigherPriorityTaskWoken;
}

/** @brief Data Register Empty Interrupt Service Routine.
 *
 *  Data Register Empty Interrupt Service Routine. Transmits one byte from TX software buffer.
 *  Disables DRE interrupt if buffer is empty.
 *
 * @param Usart software abstraction structure
 * @return xHigherPriorityTaskWoken
 */
inline signed char USART_DataRegEmpty(Usart * usart)
{
	signed char cChar, cTaskWoken;
		if( xQueueReceiveFromISR( usart->TXqueue, &cChar, &cTaskWoken ) == pdTRUE )
		{
			/* Send the next character queued for Tx. */
			usart->module->DATA = cChar;
		}
		else
		{
			/* Queue empty, nothing to send. */
		    /* Disable DRE interrupts. */
			uint8_t tempCTRLA = usart->module->CTRLA;
			tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
			usart->module->CTRLA = tempCTRLA;
		}
	return cTaskWoken;
}
