/* This file has been prepared for Doxygen automatic documentation generation.*/
/**@file *********************************************************************
 *
 * @brief  XMEGA SPI driver source file.
 *
 *      This file contains the source for the XMEGA SPI driver.
 *
 *      It is possible to have several devices on the same bus, using different
 *      SS pins.
 *
 *      Implementation relies on FreeRTOS and it is thread safe.
 *
 *		Bus contention prevention in master mode feature is not used.
 *
 *      Driver is not intended to be fast, especially in Slave mode. It
 *      concentrates the ease of understanding and use. Structures Slave and
 *      Master are not cast to void *, like it is done with xQueueHandle, for
 *      the reason to provide easier to understand code.
 *
 * @author
 *      Yuriy Kulikov yuriy.kulikov.87@gmail.com
 *
 *****************************************************************************/

#include "spi_driver.h"
// Scheduler include files.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "port_driver.h"

/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Master structure */
SpiMaster *masterC;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Slave structure */
SpiSlave *slaveC;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Master structure */
SpiMaster *masterD;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Slave structure */
SpiSlave *slaveD;
//Prototype
signed char Spi_handleInterrupt (SPI_t *module, SpiMaster *master, SpiSlave *slave);
//Interrupt handling for the module on the port C
ISR(SPIC_INT_vect){ if( Spi_handleInterrupt(&SPIC, masterC, slaveC) ) taskYIELD(); }
//Interrupt handling for the module on the port D
ISR(SPID_INT_vect){ if( Spi_handleInterrupt(&SPID, masterD, slaveD) ) taskYIELD(); }

/**
 * @brief Common for all modules interrupt handler
 * @param module pointer to the module which has requested the interrupt
 * @param master pointer to the corresponding Master structure
 * @param slave pointer to the corresponding Slave structure
 * @return true if high priority task was unblocked by queue manipulation, false if not
 * */
inline signed char Spi_handleInterrupt (SPI_t *module, SpiMaster *master, SpiSlave *slave) {
	//Used to switch tasks, if queue or semaphore manipulation should wake a task.
	signed char isHigherPriorityTaskWoken = pdFALSE;
	//If spi module was initialized as master
	if (module->CTRL & SPI_MASTER_bm){
		//TODO for arrays
	}
	//If SPI module was initialized as slave
	else {
		// Get received data.
		uint8_t receivedChar = module->DATA;
		// Post the character on the queue of received characters.
		xQueueSendToBackFromISR(slave->commandsQueue, &receivedChar, &isHigherPriorityTaskWoken );
		// Put the status
		module->DATA = slave->status;
	}
	// If the post causes a task to wake force a context switch as the waken task
	// may have a higher priority than the task we have interrupted.
	return isHigherPriorityTaskWoken;
}

/**
 * @brief Initialize SPI slave on the specified module.
 * @param module pointer to the module to use
 * @param lsbFirst
 * @param mode
 * @param bufferSize
 * @return pointer to the Slave struct, to be used with functions prefixed by SpiSlave_
 */
SpiSlave * SpiSlave_init(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, uint8_t queueSize) {
	// Allocate memory for the Slave structure and store the pointer to it
	SpiSlave *slave = ( SpiSlave * ) pvPortMalloc( sizeof( SpiSlave ) );
	// Create the queue
	slave->commandsQueue = xQueueCreate( queueSize, sizeof(char));
	// Initial magic value, could be anything
	slave->status = 0x42;
	// Used to set pins as inputs or outputs
	PORT_t *port = NULL;
	//Switch by the pointer to the hardware module to set up port and ISR-Task communication structure
	switch ((int)module) {
		case (int)&SPIC:
			//Store the pointer to the allocated structure into the appropriate global variable
			slaveC = slave;
			port = &PORTC;
			break;
		case (int)&SPID:
			//Store the pointer to the allocated structure into the appropriate global variable
			slaveD = slave;
			port = &PORTD;
			break;
		// TODO add more cases for the A series devices
		default:
			//this should never happen
			break;
		}
	port->DIRSET = SPI_MISO_bm;
	// Set up SPI hardware module
	module->CTRL = SPI_ENABLE_bm | (lsbFirst ? SPI_DORD_bm : 0) | mode;
	module->INTCTRL = SPI_INTLVL_LO_gc;
	return slave;
}

/**
 * @brief This function initializes a SPI module as master.
 * @param module The SPI module.
 * @param port The I/O port where the SPI module is connected.
 * @param lsbFirst Data order will be LSB first if this is set to a non-zero value.
 * @param mode SPI mode (Clock polarity and phase).
 * @param intLevel SPI interrupt level.
 * @param clk2x SPI double speed mode
 * @param clockDivision SPI clock pre-scaler division factor.
 * @return pointer to the SpiMaster struct, to be used to create SpiDevice structures
 */
SpiMaster * SpiMaster_init(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, bool clk2x, SPI_PRESCALER_t clockDivision){
	// Allocate memory for the Master structure and store the pointer to it
	SpiMaster *master = ( SpiMaster * ) pvPortMalloc( sizeof( SpiMaster ) );
	// Store module pointer first
	master->module=module;
	// Used to set pins as inputs or outputs
	PORT_t *port = NULL;
	//Switch by the pointer to the hardware module to set up port and ISR-Task communication structure
	switch ((int)module) {
		case (int)&SPIC:
			// SPIC interrupt vector will be triggered, connect it to the structure.
			masterC = master;
			port = &PORTC;
			break;
		case (int)&SPID:
			// SPID interrupt vector will be triggered, connect it to the structure.
			masterD = master;
			port = &PORTD;
			break;
		// TODO add more cases for the A series devices
		default:
			//this should never happen
			break;
	}
	// Set MOSI and SCK pins outputs;
	port->DIRSET = SPI_MOSI_bm | SPI_SCK_bm;
	// SS should be set to the output, see SS pin section in the XMEGA SPI application note
	port->DIRSET = SPI_SS_bm;
	// Set SS pin high
	port->OUTSET = SPI_SS_bm;
	// Set MISO input
	port->DIRCLR = SPI_MISO_bm;
	//Create binary semaphore which will be used for synchronization between task and ISR
	//vSemaphoreCreateBinary(master->semaphore);
	// Create mutex which will be used to prevent using module by several tasks at the same time.
	master->mutex = xSemaphoreCreateMutex();
	// Set up the SPI hardware module
	module->CTRL = clockDivision|(clk2x ? SPI_CLK2X_bm : 0)|SPI_ENABLE_bm
				|(lsbFirst ? SPI_DORD_bm  : 0) |SPI_MASTER_bm |mode;	// TODO Add a field to hold the interrupt level which would be used to transmit large packets
	// Do not use interrupts for single-byte transmissions
	module->INTCTRL = SPI_INTLVL_OFF_gc;
	return master;
 }

/**
 * @brief Initialize device on the SPI bus controlled by master
 * @attention if you use the pin other than the standard SS pin, make sure the standard SS pin is not an input!
 * @param SpiMaster
 * @param ssPort
 * @param ssPinMask
 * @return pointer to the SpiDevice struct, to be used with functions prefixed by SpiMaster_
 */
SpiDevice * SpiMaster_initDevice(SpiMaster *master, PORT_t *ssPort, uint8_t ssPinMask){
	// Allocate memory for the MasterDevice structure and store the pointer to it
	SpiDevice *device = ( SpiDevice * ) pvPortMalloc( sizeof( SpiDevice ) );
	// Store spi structure to use for this device
	device->master = master;
	// Store SS configuration.
	device->ssPort = ssPort;
	device->ssPinMask = ssPinMask;
	// Set SS pin output
	device->ssPort->DIRSET = ssPinMask;
	// Configure wired and pull-up
	PORT_ConfigurePins(ssPort, ssPinMask,false,false,PORT_OPC_WIREDANDPULL_gc,PORT_ISC_BOTHEDGES_gc );
	// Set SS pin high
	ssPort->OUTSET = ssPinMask;
	return device;
}

/**
 * @brief Get single byte from the slave queue, which was put there when master performed write.
 * @param slave
 * @param receivedByte
 * @param ticksToWait
 * @return true if data was fetched from the queue, false otherwise
 */
char SpiSlave_getByteFromQueue(SpiSlave *slave, uint8_t *receivedByte, int ticksToWait) {
	return xQueueReceive(slave->commandsQueue, receivedByte, ticksToWait);
}

/**
 * @brief Tries to obtain the mutex and pulls SS pin low
 * @param device
 * @param ticks To Wait to obtain the mutex
 * @return true if access to the bus was granted (mutex obtained), false otherwise
 */
char SpiMaster_startTransmission (SpiDevice *device, int ticksToWait) {
	//Try to obtain mutex
	if (xSemaphoreTake(device->master->mutex, ticksToWait)==pdTRUE) {
		// Pull SS low
		device->ssPort->OUTCLR=device->ssPinMask;
		return pdTRUE;
	}
	// Function could not obtain the mutex, so return false
	return pdFALSE;
}
/**
 * @brief Set SS pin high and release the mutex
 * @param device
 */
void SpiMaster_stopTransmission (SpiDevice *device) {
	// Pull SS high
	device->ssPort->OUTSET = device->ssPinMask;
	//Release the mutex
	xSemaphoreGive(device->master->mutex);
}

/**
 * @brief Shifts data with the slave device.
 * @attention Call SpiMaster_startTransmission() before and SpiMaster_stopTransmission() after
 * @param spiMaster
 * @param data
 * @return received value from slave
 */
uint8_t SpiMaster_shiftByte(SpiDevice *device, uint8_t data) {
	//Put byte to initialize data transmission
	device->master->module->DATA=data;
	//Wait until byte is shifted
	while(!(device->master->module->STATUS & SPI_IF_bm)) { nop(); }
	// Accessing the DATA register will clear the SPI_IF_bm
	return device->master->module->DATA;
}
