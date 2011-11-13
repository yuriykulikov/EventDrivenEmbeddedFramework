/* This file has been prepared for Doxygen automatic documentation generation.*/
/** @file *********************************************************************
 *
 * @brief  XMEGA SPI example header
 *
 *      This file contains task and function prototypes, definitions of structs,
 *      which are used to pass parameters to tasks.
 *
 * @par Documentation
 *     http://www.FreeRTOS.org - Documentation, latest information.
 *
 * @author
 *      Universität Erlangen-Nurnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 *****************************************************************************/


#include "usart_driver_RTOS.h"
#include "spi_driver.h"

/** Used to pass arguments to the task */
typedef struct SpiSlaveTaskParametersDefinition
{
	/** Pointer to SpiSlave to use */
	SpiSlave * spiSlave;
	/** Pointer to USART buffer to use */
	Usart * usartBuffer;
} SpiSlaveTaskParameters;
void startSpiSlaveTask(SpiSlave * slave, Usart * usartBuffer, char cPriority, xTaskHandle taskHandle);

