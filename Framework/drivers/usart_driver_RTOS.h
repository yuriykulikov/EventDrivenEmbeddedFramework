/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA USART driver for FreeRTOS header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA USART driver.
 *
 *		Functions can be used with or without the kernel running. All operations are
 *		thread safe.
 *
 *      The driver is not intended for size and/or speed critical code, focusing
 *      on the ease of understanding and flexibility instead.
 *      This file is based on the driver provided by Atmel Corporation and drivers
 *      provided by FreeRTOS.org
 *
 * \author
 * Yuriy Kulikov
 *
 *****************************************************************************/
#ifndef USART_DRIVER_H
#define USART_DRIVER_H
#include "avr_compiler.h"
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {
	BAUD9600 = 1,
	BAUD19200 = 2,
} Baudrate_enum;

#define DONT_BLOCK 0
#define MAX_DELAY portMAX_DELAY

/*! \brief Struct used when interrupt driven driver is used.
*  Struct containing pointer to a usart, a buffer and a location to store Data
*  register interrupt level temporary.
*/
typedef struct UsartStructDefenition
{
	/* \brief Pointer to USART module to use. */
	USART_t * usart;
	/* \brief Data register empty interrupt level. */
	USART_DREINTLVL_t dreIntLevel;
	/* \brief Data buffer. */
	xQueueHandle xQueueRX;
	xQueueHandle xQueueTX;
} USART_buffer_struct_t;

/* Functions for interrupt driven driver. */

/*! \brief This function is a "constructor", it allocates memory,
 *  makes all initialization according to input values, enables interrupts and all.
 *  \return pointer to the serial
 */
USART_buffer_struct_t * USART_InterruptDriver_Initialize(USART_t * usart, Baudrate_enum baudrate ,char bufferSize);

void USART_Buffer_PutByte(USART_buffer_struct_t * usart_buffer_t, uint8_t data, int ticksToWait );
void USART_Buffer_PutString(USART_buffer_struct_t * usart_buffer_t, const char *string, int ticksToWait );
void USART_Buffer_PutInt(USART_buffer_struct_t * usart_buffer_t, int16_t Int,int16_t radix, int ticksToWait );
int8_t USART_Buffer_GetByte(USART_buffer_struct_t * usart_buffer_t, char * receivedChar, int ticksToWait );

#endif

