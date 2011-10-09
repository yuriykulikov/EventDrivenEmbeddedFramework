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
#include "semphr.h"

// Framework
#include "usart_driver_RTOS.h"
#include "led.h"
#include "spi_driver.h"
#include <string.h>
#include "strings.h"
// Utils includes
#include "CommandInterpreter.h"
// File headers
#include "spi_task.h"

xSemaphoreHandle spiMasterTaskSemaphore;

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
			Usart_putPgmString(usartBuffer,Strings_SpiSlaveExample1,10);
			Usart_putInt(usartBuffer,receivedChar,16,10);
			Usart_putPgmString(usartBuffer, Strings_newline, 10);
			// report some kind of status
			slave->status = receivedChar + 0x01;
		}
	}
}

/**
 * @brief Master test task
 * Task executes several write and read operations after every certain amount of time.
 * @attention use @link startSpiMasterTask
 * @param pvParameters pass the struct using xTaskCreate
 */
void SpiMasterTask( void *pvParameters )
{
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	SpiMasterTaskParameters * parameters = (SpiMasterTaskParameters *)pvParameters;
	//store pointer to usart for convenience
	Usart * usartBuffer = parameters->usartBuffer;
	SpiDevice * master = parameters->spiMaster;
	uint8_t receivedChar='#';
	//Infinite loop
	for(;;) {
		if (xSemaphoreTake(spiMasterTaskSemaphore, portMAX_DELAY)) {
		char obtainedMutex = SpiMaster_startTransmission(master, 10);
			if (obtainedMutex) {
				//Transmit bytes
				Usart_putPgmString(usartBuffer, Strings_SpiMasterExample1, 10);
				receivedChar = SpiMaster_shiftByte(master, 0xC0);
				Usart_putInt(usartBuffer,receivedChar,16,10);
				receivedChar = SpiMaster_shiftByte(master, 0x01);
				Usart_putInt(usartBuffer,receivedChar,16,10);
				Usart_putPgmString(usartBuffer, Strings_newline, 10);
				//Transmit more bytes
				vTaskDelay(1);
				Usart_putPgmString(usartBuffer, Strings_SpiMasterExample2, 10);
				receivedChar = SpiMaster_shiftByte(master, 0xC0);
				Usart_putInt(usartBuffer,receivedChar,16,10);
				receivedChar = SpiMaster_shiftByte(master, 0xDE);
				Usart_putInt(usartBuffer,receivedChar,16,10);
				Usart_putPgmString(usartBuffer, Strings_newline, 10);
				//Transmit more bytes
				vTaskDelay(500);
				Usart_putPgmString(usartBuffer, Strings_SpiMasterExample3, 10);
				receivedChar = SpiMaster_shiftByte(master, 0xD0);
				Usart_putInt(usartBuffer,receivedChar,16,10);
				receivedChar = SpiMaster_shiftByte(master, 0x00);
				Usart_putInt(usartBuffer,receivedChar,16,10);
				receivedChar = SpiMaster_shiftByte(master, 0xDE);
				Usart_putInt(usartBuffer,receivedChar,16,10);
				Usart_putPgmString(usartBuffer, Strings_newline, 10);
				SpiMaster_stopTransmission(master);
			}
		}
	}
}

/**
 * Gives semaphore, so the task executes
 * @param pcWriteBuffer
 * @param writeBufferLen
 * @return
 */
static portBASE_TYPE giveSpiMasterTaskSemaphore( signed char *writeBuffer, size_t writeBufferLen ) {
	// Give semaphore to the task to be executed once
	xSemaphoreGive(spiMasterTaskSemaphore);
	strncpy_P( (char*) writeBuffer, Strings_SpiExampleCmdResp, writeBufferLen );
	return pdFALSE;
}

/**
 * @brief Starts Master test task
 * @param spiMaster
 * @param usartBuffer
 * @param cPriority
 * @param taskHandle
 */
void startSpiMasterTask(SpiDevice * master, Usart * usartBuffer, char cPriority, xTaskHandle taskHandle){

	//Task will receive these parameters later, they should be either allocated dynamically or be static
	SpiMasterTaskParameters * spiMasterTaskParameters = pvPortMalloc(sizeof(SpiMasterTaskParameters));
	spiMasterTaskParameters->spiMaster=master;
	spiMasterTaskParameters->usartBuffer=usartBuffer;
	// This semaphore is used to launch task when command is received
	vSemaphoreCreateBinary (spiMasterTaskSemaphore);
	// Take the semaphore once to clear it
	xSemaphoreTake(spiMasterTaskSemaphore, 0);
	// Registed a command for the interpreter
	xCmdIntRegisterCommand(Strings_SpiExampleCmd, Strings_SpiExampleCmdDesc, giveSpiMasterTaskSemaphore);
	// Spawn task, stack size is approximate, seems to work well
	xTaskCreate(SpiMasterTask, ( signed char * ) "SPIMASTER", 250, spiMasterTaskParameters, cPriority, NULL );
}
