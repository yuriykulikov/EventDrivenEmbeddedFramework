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
#include "led.h"
#include "spi_driver.h"
// File headers
#include "spi_task.h"
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
			Usart_putString(usartBuffer,"Slave received: 0x",10);
			Usart_putInt(usartBuffer,receivedChar,16,10);
			Usart_putString(usartBuffer, "\n", 10);
			// report some kind of status
			slave->status = receivedChar + 0x01;
		}
	}
}

//Prototype
void SpiMasterTask( void *pvParameters );
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

	// Spawn task, stack size is approximate, seems to work well
	xTaskCreate(SpiMasterTask, ( signed char * ) "SPIMASTER", 250, spiMasterTaskParameters, cPriority, NULL );
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

		vTaskDelay(2000);
		char obtainedMutex = SpiMaster_startTransmission(master, 10);
		if (obtainedMutex) {
			//Transmit one byte
			receivedChar = SpiMaster_shiftByte(master, 0xEE);
			Usart_putString(usartBuffer, "Master received: 0x", 10);
			Usart_putInt(usartBuffer,receivedChar,16,10);
			//Transmit one more byte
			receivedChar = SpiMaster_shiftByte(master, 0x88);
			Usart_putString(usartBuffer, " , 0x", 10);
			Usart_putInt(usartBuffer,receivedChar,16,10);
			Usart_putString(usartBuffer, "\n", 10);
			SpiMaster_stopTransmission(master);
		}
	}
}
