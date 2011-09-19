/* This file has been prepared for Doxygen automatic documentation generation.*/
/**@file *********************************************************************
 *
 * @brief  XMEGA SPI driver header file.
 *
 *      This file contains the function prototypes and structure definitions
 *      for the XMEGA SPI driver.
 *
 *      It is possible to have several devices on the same bus, using different
 *      SS pins.
 *
 *      Implementation relies on FreeRTOS and it is thread safe.
 *
 *      Driver is not intended to be fast, especially in Slave mode. It
 *      concentrates the ease of understanding and use. Structures SpiSlave and
 *      SpiMaster are not cast to void *, like it is done with xQueueHandle, for
 *      the reason to provide easier to understand code.
 *
 * @author
 *      Yuriy Kulikov yuriy.kulikov.87@gmail.com
 *
 *****************************************************************************/
#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "avr_compiler.h"

// Scheduler include files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define SPI_SS_bm             0x10
#define SPI_MOSI_bm           0x20
#define SPI_MISO_bm           0x40
#define SPI_SCK_bm            0x80

/**
 * @brief Structure, representing an SPI master.
 * It is used to pass data between Task and ISR.
 * There should be one global instance of this struct for each module.
 * @attention  Interact with this struct only using functions.
 */
typedef struct MasterDefinition {
	/** Pointer to what module to use. */
	SPI_t *module;
	//Master specific fields
	/** Used to prevent several tasks from using SPI module simultaneously */
	xSemaphoreHandle mutex;
	/** Used to synchronize task with interrupts*/
//	xSemaphoreHandle semaphore;
	/** Pointer to data to transmit */
//	const uint8_t *transmitData;
	/** Pointer to receiving buffer */
//	volatile uint8_t *receiveData;
//	volatile uint8_t bytesToTransceive;
//	volatile uint8_t bytesTransceived;
//	volatile uint8_t bytesToReceive;
//	volatile uint8_t bytesReceived;
} SpiMaster;

/**
 * @brief Represents particular device on the bus
 * Holds pointer to the SPI master, and SS pin description. Initialize one for each device on SPI bus.
 * @attention  Interact with this struct only using functions.
 */
typedef struct MasterDeviceDefinition {
	/** Spi structure to use */
	SpiMaster *master;
	//SS pin description
	PORT_t *ssPort;
	uint8_t ssPinMask;
} SpiDevice;

/**
 * @brief Represents the slave.
 * @attention  Interact with this struct only using functions.
 */
typedef struct SpiSlaveDefinition {
	/** Contents of this register are sent to the master when not transmitting data*/
	uint8_t status;
	/** Master out slave in commands queue */
	xQueueHandle commandsQueue;
} SpiSlave;

SpiMaster * SpiMaster_init(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, bool clk2x, SPI_PRESCALER_t clockDivision);
SpiDevice * SpiMaster_initDevice(SpiMaster *master, PORT_t *ssPort, uint8_t ssPinMask);

char SpiMaster_startTransmission (SpiDevice *device, int ticksToWait);
uint8_t SpiMaster_shiftByte(SpiDevice *master, uint8_t data);
void SpiMaster_stopTransmission (SpiDevice *device);

SpiSlave * SpiSlave_init(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, uint8_t queueSize);

char SpiSlave_getByteFromQueue(SpiSlave *slave, uint8_t * receivedByte, int ticksToWait);

#endif
