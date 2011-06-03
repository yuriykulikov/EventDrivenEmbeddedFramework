/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      XMEGA SPI driver source file.
 *
 *      This file contains the function implementations the XMEGA SPI driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA SPI module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is discouraged,
 *      in some occasions the operator makes it possible to write pretty clean and
 *      neat code. In this driver, the construct is used to set or not set a
 *      configuration bit based on a boolean input parameter, such as
 *      the "some_parameter" in the example above.
 *
 * \par Application note:
 *      AVR1309: Using the XMEGA SPI
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 764 $
 * $Date: 2007-11-06 14:52:26 +0100 (ti, 06 nov 2007) $  \n
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
#include "spi_driver.h"



/*! \brief Initialize SPI module as master.
 *
 *  This function initializes a SPI module as master. The CTRL and INTCTRL
 *  registers for the SPI module is set according to the inputs to the function.
 *  In addition, data direction for the MOSI and SCK pins is set to output.
 *
 *  \param spi            The SPI_Master_t struct instance.
 *  \param module         The SPI module.
 *  \param port           The I/O port where the SPI module is connected.
 *  \param lsbFirst       Data order will be LSB first if this is set to a
 *                        non-zero value.
 *  \param mode           SPI mode (Clock polarity and phase).
 *  \param intLevel       SPI interrupt level.
 *  \param clk2x	      SPI double speed mode
 *  \param clockDivision  SPI clock prescaler divison factor.
 */
void SPI_MasterInit(SPI_Master_t *spi,
                    SPI_t *module,
                    PORT_t *port,
                    bool lsbFirst,
                    SPI_MODE_t mode,
                    SPI_INTLVL_t intLevel,
                    bool clk2x,
                    SPI_PRESCALER_t clockDivision)
{
	spi->module         = module;
	spi->port           = port;
	spi->interrupted    = false;

	spi->module->CTRL   = clockDivision |                  /* SPI prescaler. */
	                      (clk2x ? SPI_CLK2X_bm : 0) |     /* SPI Clock double. */
	                      SPI_ENABLE_bm |                  /* Enable SPI module. */
	                      (lsbFirst ? SPI_DORD_bm  : 0) |  /* Data order. */
	                      SPI_MASTER_bm |                  /* SPI master. */
	                      mode;                            /* SPI mode. */

	/* Interrupt level. */
	spi->module->INTCTRL = intLevel;

	/* No assigned data packet. */
	spi->dataPacket = NULL;

 	/* MOSI and SCK as output. */
	spi->port->DIRSET  = SPI_MOSI_bm | SPI_SCK_bm;
}



/*! \brief Initialize SPI module as slave.
 *
 *  This function initializes a SPI module as slave. The CTRL and INTCTRL
 *  registers for the SPI module is set according to the inputs to the function.
 *  In addition, data direction for the MISO pin is set to output.
 *
 *  \param spi                  The SPI_Slave_t instance.
 *  \param module               Pointer to the SPI module.
 *  \param port                 The I/O port where the SPI module is connected.
 *  \param lsbFirst             Data order will be LSB first if this is set to true.
 *  \param mode                 SPI mode (Clock polarity and phase).
 *  \param intLevel             SPI interrupt level.
 */
void SPI_SlaveInit(SPI_Slave_t *spi,
                   SPI_t *module,
                   PORT_t *port,
                   bool lsbFirst,
                   SPI_MODE_t mode,
                   SPI_INTLVL_t intLevel)
{
	/* SPI module. */
	spi->module       = module;
	spi->port         = port;

	spi->module->CTRL = SPI_ENABLE_bm |                /* Enable SPI module. */
	                    (lsbFirst ? SPI_DORD_bm : 0) | /* Data order. */
	                    mode;                          /* SPI mode. */

	/* Interrupt level. */
	spi->module->INTCTRL = intLevel;

	/* MISO as output. */
	spi->port->DIRSET = SPI_MISO_bm;
}



/*! \brief Create data packet.
 *
 *  This function prepares a data packet for transmission. Note that memory
 *  for dataPacket, transmitData and receiveData must be allocated outside this
 *  function.
 *
 *  \note The size of the receiveData and transmitData must be equal and
 *        bytesToTransceive should be set accordingly.
 *
 *  \param dataPacket         Pointer to data packet used for this transmission.
 *  \param transmitData       Pointer to data to transmit.
 *  \param receiveData        Pointer to receive buffer.
 *  \param bytesToTransceive  The number of bytes to transmit/receive.
 *  \param ssPort             Pointer to I/O port where the SS pin used for this
 *                            transmission is located.
 *  \param ssPinMask          Pin mask selecting the SS pin in ssPort.
 */
void SPI_MasterCreateDataPacket(SPI_DataPacket_t *dataPacket,
                                const uint8_t *transmitData,
                                uint8_t *receiveData,
                                uint8_t bytesToTransceive,
                                PORT_t *ssPort,
                                uint8_t ssPinMask)
{
	dataPacket->ssPort            = ssPort;
	dataPacket->ssPinMask         = ssPinMask;
	dataPacket->transmitData      = transmitData;
	dataPacket->receiveData       = receiveData;
	dataPacket->bytesToTransceive  = bytesToTransceive;
	dataPacket->bytesTransceived   = 0;
	dataPacket->complete          = false;
}



/*! \brief Common SPI master interrupt service routine.
 *
 *  This function is called by the SPI interrupt service handlers. For each
 *  SPI module that uses this driver, the ISR should call this function with
 *  a pointer to the related SPI_Master_t struct as argument.
 *
 *  \param spi        Pointer to the modules own SPI_Master_t struct.
 */
void SPI_MasterInterruptHandler(SPI_Master_t *spi)
{
	uint8_t data;
	uint8_t bytesTransceived = spi->dataPacket->bytesTransceived;

	/* If SS pin interrupt (SS used and pulled low).
	*  No data received at this point. */
	if ( !(spi->module->CTRL & SPI_MASTER_bm) ) {
		spi->interrupted = true;
	}

	else {  /* Data interrupt. */

		/* Store received data. */
		data = spi->module->DATA;
		spi->dataPacket->receiveData[bytesTransceived] = data;

		/* Next byte. */
		bytesTransceived++;

		/* If more data. */
		if (bytesTransceived < spi->dataPacket->bytesToTransceive) {
			/* Put data byte in transmit data register. */
			data = spi->dataPacket->transmitData[bytesTransceived];
			spi->module->DATA = data;
		}

		/* Transmission complete. */
		else {

			/* Release SS to slave(s). */
			uint8_t ssPinMask = spi->dataPacket->ssPinMask;
			SPI_MasterSSHigh(spi->dataPacket->ssPort, ssPinMask);

			spi->dataPacket->complete = true;
		}
	}
	/* Write back bytesTransceived to data packet. */
	spi->dataPacket->bytesTransceived = bytesTransceived;
}



/*! \brief Start transmission.
 *
 *  This function starts a SPI transmission. A data packet must be prepared
 *  for transmission first.
 *
 *  \param spi                The SPI_Master_t struct instance.
 *  \param dataPacket         The SPI_dataPacket_t struct instance.
 *
 *  \return                   Status code
 *  \retval SPI_OK            The transmission was completed successfully.
 *  \retval SPI_BUSY          The SPI module is busy.
 *  \retval SPI_INTERRUPTED   The transmission was interrupted by another master.
 */
uint8_t SPI_MasterInterruptTransceivePacket(SPI_Master_t *spi,
                                            SPI_DataPacket_t *dataPacket)
{
	uint8_t data;
	bool interrupted = spi->interrupted;

	/* If no packets sent so far. */
	if (spi->dataPacket == NULL) {
		spi->dataPacket = dataPacket;
	}

	/* If ongoing transmission. */
	else if (spi->dataPacket->complete == false) {
		return (SPI_BUSY);
	}

	/* If interrupted by other master. */
	else if (interrupted) {
		/* If SS released. */
		if (spi->port->OUT & SPI_SS_bm) {
			/* No longer interrupted. */
			interrupted = false;
		}
		else {
			return (SPI_INTERRUPTED);
		}
	}

	/* NOT interrupted by other master.
	* Start transmission. */
	spi->dataPacket = dataPacket;
	spi->dataPacket->complete = false;
	spi->interrupted = false;

	/* SS to slave(s) low.*/
	uint8_t ssPinMask = spi->dataPacket->ssPinMask;
	SPI_MasterSSLow(spi->dataPacket->ssPort, ssPinMask);

	spi->dataPacket->bytesTransceived = 0;

	/* Start sending data. */
	data = spi->dataPacket->transmitData[0];
	spi->module->DATA = data;

	/* Successs */
	return (SPI_OK);
}


/*! \brief SPI mastertransceive byte
 *
 *  This function clocks data in the DATA register to the slave, while data
 *  from the slave is clocked into the DATA register. The function does not
 *  check for ongoing access from other masters before initiating a transfer.
 *  For multimaster systems, checkers should be added to avoid bus contention.
 *
 *  SS line(s) must be pulled low before calling this function and released
 *  when finished.
 *
 *  \note This function is blocking and will not finish unless a successful
 *        transfer has been completed. It is recommended to use the
 *        interrupt-driven driver for applications where blocking
 *        functionality is not wanted.
 *
 *  \param spi        The SPI_Master_t struct instance.
 *  \param TXdata     Data to transmit to slave.
 *
 *  \return           Data received from slave.
 */
uint8_t SPI_MasterTransceiveByte(SPI_Master_t *spi, uint8_t TXdata)
{
	/* Send pattern. */
	spi->module->DATA = TXdata;

	/* Wait for transmission complete. */
	while(!(spi->module->STATUS & SPI_IF_bm)) {

	}
	/* Read received data. */
	uint8_t result = spi->module->DATA;

	return(result);
}



/*! \brief SPI transceive data packet
 *
 *  This function transceives a number of bytes contained in a data packet
 *  struct. The SS line is kept low until all bytes are transceived. The
 *  received bytes are stored in the data packet struct.
 *
 *  \param spi         The SPI_Master_t struct instance.
 *  \param dataPacket  The SPI_dataPacket_t struct instance.
 *
 *  \return            Wether the function was successfully completed
 *  \retval true	   Success
 *  \retval false	   Failure
 */
bool SPI_MasterTransceivePacket(SPI_Master_t *spi,
                                SPI_DataPacket_t *dataPacket)
{
	/* Check if data packet has been created. */
	if(dataPacket == NULL) {
		return false;
	}

	/* Assign datapacket to SPI module. */
	spi->dataPacket = dataPacket;

	uint8_t ssPinMask = spi->dataPacket->ssPinMask;

	/* If SS signal to slave(s). */
	if (spi->dataPacket->ssPort != NULL) {
		/* SS to slave(s) low. */
		SPI_MasterSSLow(spi->dataPacket->ssPort, ssPinMask);
	}

	/* Transceive bytes. */
	uint8_t bytesTransceived = 0;
	uint8_t bytesToTransceive = dataPacket->bytesToTransceive;
	while (bytesTransceived < bytesToTransceive) {

		/* Send pattern. */
		uint8_t data = spi->dataPacket->transmitData[bytesTransceived];
		spi->module->DATA = data;

		/* Wait for transmission complete. */
		while(!(spi->module->STATUS & SPI_IF_bm)) {

		}
		/* Read received data. */
		data = spi->module->DATA;
		spi->dataPacket->receiveData[bytesTransceived] = data;

		bytesTransceived++;
	}

	/* If SS signal to slave(s). */
	if (spi->dataPacket->ssPort != NULL) {
		/* Release SS to slave(s). */
		SPI_MasterSSHigh(spi->dataPacket->ssPort, ssPinMask);
	}

	/* Set variables to indicate that transmission is complete. */
	spi->dataPacket->bytesTransceived = bytesTransceived;
	spi->dataPacket->complete = true;

	/* Report success. */
	return true;
}
