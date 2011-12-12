/* This file has been prepared for Doxygen automatic documentation generation.*/
/**@file *********************************************************************
 *
 * @brief  XMEGA SPI example source file.
 *
 *
 * @author
 *      Yuriy Kulikov yuriy.kulikov.87@gmail.com
 *
 *****************************************************************************/
// Compiler definitions include file
#include "avr_compiler.h"

// Scheduler include files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Framework
#include "usart_driver_RTOS.h"
#include "spi_driver.h"

// File headers
#include "SpiSlaveTask.h"
#include "strings.h"

//Prototype
void SpiSlaveTask( void *pvParameters );
/**
 * @brief Starts SPI Slave example task
 * @param usartBuffer
 * @param debugLed
 * @param cPriority
 * @param taskHandle
 */
void startSpiSlaveTask(SpiSlave * slave, Usart * usartBuffer, char cPriority, xTaskHandle taskHandle)
{
	//Put parameters into the struct
	SpiSlaveTaskParameters * spiSlaveTaskParameters = pvPortMalloc(sizeof(SpiSlaveTaskParameters));
	spiSlaveTaskParameters->spiSlave=slave;
	spiSlaveTaskParameters->usartBuffer=usartBuffer;

	// Spawn task, stack size is approximate, seems to work well
	xTaskCreate(SpiSlaveTask, ( signed char * ) "SPISLAVE", 150, spiSlaveTaskParameters, cPriority, NULL );
}
/**
 * @brief Slave task - simple spi-usart bridge
 * Tasks blocks on the queue, and as soon as data appears on the queue, task sends it
 * over the usart.
 * @attention use @link startSpiSlaveTask
 * @param pvParameters pass the struct using xTaskCreate
 */
void SpiSlaveTask( void *pvParameters ) {
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	SpiSlaveTaskParameters * parameters = (SpiSlaveTaskParameters *)pvParameters;
	//store pointer to usart for convenience
	Usart * usartBuffer = parameters->usartBuffer;
	SpiSlave * slave = parameters->spiSlave;
	uint8_t receivedChar='#';

	//Infinite loop
	for (;;) {
		//Function will block the task
		if (SpiSlave_getByteFromQueue(slave, &receivedChar, portMAX_DELAY) == pdPASS )
		{
			Usart_putPgmStringDflt(usartBuffer,Strings_SpiSlaveExample1);
			Usart_putIntDflt(usartBuffer,receivedChar,16);
			Usart_putPgmStringDflt(usartBuffer, Strings_newline);
			// report some kind of status
			slave->status = receivedChar + 0x01;
		}
	}
}
