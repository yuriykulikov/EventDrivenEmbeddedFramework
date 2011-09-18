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
Master * masterC;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Slave structure */
Slave * slaveC;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Master structure */
Master * masterD;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Slave structure */
Slave * slaveD;
//Prototype
signed char Spi_handleInterrupt (SPI_t *module, Master * master, Slave * slave);
//Interrupt handling for the module on the port C
ISR(SPIC_INT_vect){ if( Spi_handleInterrupt(&SPIC, masterC, slaveC) ) taskYIELD(); }
//Interrupt handling for the module on the port D
ISR(SPID_INT_vect){ if( Spi_handleInterrupt(&SPID, masterD, slaveD) ) taskYIELD(); }
/**
 * @param module pointer to the module which has requested the interrupt
 * @param master pointer to the corresponding Master structure
 * @param slave pointer to the corresponding Slave structure
 * @return true if high priority task was unblocked by queue manipulation, false if not.
 */
inline signed char Spi_handleInterrupt (SPI_t *module, Master * master, Slave * slave) {
	//Used to switch tasks, if queue or semaphore manipulation should wake a task.
	signed char isHigherPriorityTaskWoken = pdFALSE;
	//If spi module was initialized as master
	if (module->CTRL & SPI_MASTER_bm){
		//TODO for arrays
	}
	//If spi module was initialized as slave
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
 * Initialize SPI slave on the specified module.
 * @param module pointer to the module to use
 * @param lsbFirst
 * @param mode
 * @param bufferSize
 * @return Spi * pointer to Spi struct initialized as slave
 */
Slave * SpiSlave_init(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, uint8_t queueSize) {
	// Allocate memory for the Slave structure and store the pointer to it
	Slave * slave = ( Slave * ) pvPortMalloc( sizeof( Slave ) );
	// Set up Spi hardware module
	module->CTRL = SPI_ENABLE_bm | (lsbFirst ? SPI_DORD_bm : 0) | mode;
	module->INTCTRL = SPI_INTLVL_LO_gc;
	// Create the queue
	slave->commandsQueue = xQueueCreate( queueSize, sizeof(char));
	// Initial magic value, could be anything
	slave->status = 0x42;
	//Switch by the pointer to the hardware module to set up port and ISR-Task communication structure
	switch ((int)module) {
		case (int)&SPIC:
			//Store the pointer to the allocated structure into the appropriate global variable
			slaveC = slave;
			//Since SPI is on the port C, we will need to use PORTC, Set MISO out
			PORTC.DIRSET = SPI_MISO_bm;
			break;
		case (int)&SPID:
			//Store the pointer to the allocated structure into the appropriate global variable
			slaveD = slave;
			//Since SPI is on the port D, we will need to use PORTD, Set MISO out
			PORTD.DIRSET = SPI_MISO_bm;
			break;
		// TODO add more cases for the A series devices
		default:
			//this should never happen
			break;
		}
	return slave;
}
/**@brief This function initializes a SPI module as master.
 *
 * @param spi The SPI_Master_t struct instance.
 * @param module The SPI module.
 * @param port The I/O port where the SPI module is connected.
 * @param lsbFirst Data order will be LSB first if this is set to a non-zero value.
 * @param mode SPI mode (Clock polarity and phase).
 * @param intLevel SPI interrupt level.
 * @param clk2x SPI double speed mode
 * @param clockDivision SPI clock prescaler divison factor.
 */
Master * SpiMaster_init(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, bool clk2x, SPI_PRESCALER_t clockDivision){
	// Allocate memory for the Master structure and store the pointer to it
	Master * master = ( Master * ) pvPortMalloc( sizeof( Master ) );
	// Store module pointer first
	master->module=module;
	// Set up Spi hardware module
	module->CTRL = clockDivision|(clk2x ? SPI_CLK2X_bm : 0)|SPI_ENABLE_bm
				|(lsbFirst ? SPI_DORD_bm  : 0) |SPI_MASTER_bm |mode;
	// TODO Add a field to hold the interrupt level which would be used to tranceive packets
	// Do not use interrupts for single-byte transmissions
	module->INTCTRL = SPI_INTLVL_OFF_gc;

	//Create binary semaphore which will be used for syncronization between task and ISR
	//vSemaphoreCreateBinary(master->semaphore);
	// Create mutex which will be used to prevent using module by several tasks at the same time.
	master->mutex = xSemaphoreCreateMutex();
	//Switch by the pointer to the hardware module to set up port and ISR-Task communication structure
	switch ((int)module) {
		case (int)&SPIC:
			// SPIC interrupt vector will be triggered, connect it to the structure.
			masterC = master;
			// Set MOSI and SCK pins outputs;
			PORTC.DIRSET  = SPI_MOSI_bm | SPI_SCK_bm;
			// Set Miso input
			PORTC.DIRCLR = SPI_MISO_bm;
			break;
		case (int)&SPID:
			// SPID interrupt vector will be triggered, connect it to the structure.
			masterD = master;
			// Set MOSI and SCK pins outputs;
			PORTD.DIRSET  = SPI_MOSI_bm | SPI_SCK_bm;
			// Set Miso input
			PORTD.DIRCLR = SPI_MISO_bm;
			break;
		// TODO add more cases for the A series devices
		default:
			//this should never happen
			break;
	}
	return master;
 };
/**
 * Initialize device on SPI bus.
 * @attention if you use the pin otherthan the standard SS pin, make sure the standard SS pin is not an input!
 * @param spi Master module to use
 * @param ssPort
 * @param ssPinMask
 * @return
 */
MasterDevice * SpiMaster_initDevice(Master *master, PORT_t *ssPort, uint8_t ssPinMask){
	// Allocate memory for the MasterDevice structure and store the pointer to it
	MasterDevice * masterDevice = ( MasterDevice * ) pvPortMalloc( sizeof( MasterDevice ) );
	// Store spi structure to use for this device
	masterDevice->master=master;
	// Store SS configuration.
	masterDevice->ssPort=ssPort;
	masterDevice->ssPinMask=ssPinMask;
	// Set SS pin output
	masterDevice->ssPort->DIRSET = ssPinMask;
	// Configure wired and pullup
	PORT_ConfigurePins(ssPort, ssPinMask,false,false,PORT_OPC_WIREDANDPULL_gc,PORT_ISC_BOTHEDGES_gc );
	// Set SS pin high
	ssPort->OUTSET = ssPinMask;
	return masterDevice;
}

/**
 * Get single byte from the slave queue, which was put there when master performed write.
 * @param slave
 * @param receivedByte
 * @param ticksToWait
 * @return Success
 */
char SpiSlave_getByteFromQueue(Slave * slave, uint8_t * receivedByte, int ticksToWait) {
	return xQueueReceive(slave->commandsQueue, receivedByte, ticksToWait);
}

/**
 * Tries to obtain the mutex and pulls SS pin low
 * @param masterDevice
 * @param ticks To Wait to obtain the mutex
 * @return true if access to the bus was granted (mutex obtained)
 */
char SpiMaster_startTransmission (MasterDevice * masterDevice, int ticksToWait) {
	//Try to obtain mutex
	if (xSemaphoreTake(masterDevice->master->mutex, ticksToWait)==pdTRUE) {
		// Pull SS low
		masterDevice->ssPort->OUTCLR=masterDevice->ssPinMask;
		return pdTRUE;
	}
	// Function could not obtain the mutex, so return false
	return pdFALSE;
}
/**
 * Set SS pin high and release the mutex
 * @param masterDevice
 */
void SpiMaster_stopTransmission (MasterDevice * masterDevice) {
	// Pull SS high
	masterDevice->ssPort->OUTSET = masterDevice->ssPinMask;
	//Release the mutex
	xSemaphoreGive(masterDevice->master->mutex);
}

/**
 * Shifts data with the slave device.
 * @attention Call SpiMaster_startTransmission() before and SpiMaster_stopTransmission() after.
 * @param spiMaster
 * @param data
 * @return received value
 */
uint8_t SpiMaster_shiftByte(MasterDevice * masterDevice, uint8_t data) {
	//Put byte to initialize data transmission
	masterDevice->master->module->DATA=data;
	//Wait until byte is shifted
	while(!(masterDevice->master->module->STATUS & SPI_IF_bm)) { nop(); }
	// Accessing the DATA register will clear the SPI_IF_bm
	return masterDevice->master->module->DATA;
}
