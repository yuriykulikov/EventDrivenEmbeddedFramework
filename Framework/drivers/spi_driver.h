/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA SPI driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA SPI driver.
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
 * $Revision: 765 $
 * $Date: 2007-11-06 14:53:04 +0100 (ti, 06 nov 2007) $  \n
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
#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "avr_compiler.h"

/* Hardware defines */

#define SPI_SS_bm             0x10 /*!< \brief Bit mask for the SS pin. */
#define SPI_MOSI_bm           0x20 /*!< \brief Bit mask for the MOSI pin. */
#define SPI_MISO_bm           0x40 /*!< \brief Bit mask for the MISO pin. */
#define SPI_SCK_bm            0x80 /*!< \brief Bit mask for the SCK pin. */

/* SPI master status code defines. */

#define SPI_OK              0     /*!< \brief The transmission completed successfully. */
#define SPI_INTERRUPTED     1     /*!< \brief The transmission was interrupted by another master. */
#define SPI_BUSY            2     /*!< \brief The SPI module is busy with another transmission. */


/*! \brief SPI data packet struct. */
typedef struct SPI_DataPacket
{
	PORT_t *ssPort;                     /*!< \brief Pointer to SS port. */
	uint8_t ssPinMask;                  /*!< \brief SS pin mask. */
	const uint8_t *transmitData;        /*!< \brief Pointer to data to transmit. */
	volatile uint8_t *receiveData;      /*!< \brief Pointer to where to store received data. */
	volatile uint8_t bytesToTransceive; /*!< \brief Number of bytes to transfer. */
	volatile uint8_t bytesTransceived;  /*!< \brief Number of bytes transfered. */
	volatile bool complete;             /*!< \brief Complete flag. */
} SPI_DataPacket_t;


/*! \brief SPI master struct. Holds pointer to SPI module, buffers and necessary varibles. */
typedef struct SPI_Master
{
	SPI_t *module;                /*!< \brief Pointer to what module to use. */
	PORT_t *port;                 /*!< \brief Pointer to port. */
	bool interrupted;             /*!< \brief True if interrupted by other master (SS pulled low). */
	SPI_DataPacket_t *dataPacket; /*!< \brief Holds transceive data. */
} SPI_Master_t;


/*! \brief SPI slave struct. Holds pointers to SPI module and used port. */
typedef struct SPI_Slave
{
	SPI_t *module;      /*!< \brief Pointer to what module to use. */
	PORT_t *port;       /*!< \brief Pointer to port. */
} SPI_Slave_t;


/* Definitions of macros. */


/*! \brief Checks if transmission is complete.
 *
 *  \param _spi     Pointer to SPI_Master_t struct instance.
 *
 *  \return         The current status of the transmission.
 *  \retval true    The transmission is complete.
 *  \retval false   The transmission is in progress.
 */
#define SPI_MasterInterruptTransmissionComplete(_spi) ( (_spi)->dataPacket->complete )



/*! \brief Pulls SPI SS line(s) low in order to address the slave devices.
 *
 *  \param _port         Pointer to the I/O port where the SS pins are located.
 *  \param _pinBM        A bitmask selecting the pins to pull low.
 *
 *  \retval NA
 */
#define SPI_MasterSSLow(_port, _pinBM) ( (_port)->OUTCLR = (_pinBM) )



/*! \brief Releases SPI SS line(s).
 *
 *  \param _port         Pointer to the I/O port where the SS pins are located.
 *  \param _pinBM        A bitmask selecting the pins to release.
 *
 *  \retval NA
 */
#define SPI_MasterSSHigh(_port, _pinBM) ( (_port)->OUTSET = (_pinBM) )



/*! \brief Write data byte to the SPI shift register.
 *
 *  \param _spi        Pointer to SPI_Slave_t struct instance.
 *  \param _data       The data to write to the shift register.
 */
#define SPI_SlaveWriteByte(_spi, _data) ( (_spi)->module->DATA = (_data) )



/*! \brief Read received data byte.
 *
 *  \param _spi       Pointer to SPI_Slave_t struct instance.
 *
 *  \return           The received data.
 */
#define SPI_SlaveReadByte(_spi) ( (_spi)->module->DATA )



/*! \brief Check if new data is available.
 *
 *  \param _spi       Pointer to SPI_Slave_t struct instance.
 *
 *  \return           True if data available, false if not.
 */
#define SPI_SlaveDataAvailable(_spi) ( (_spi)->module->STATUS & SPI_IF_bm )


/* Prototype functions. Documentation found in source file */

void SPI_MasterInit(SPI_Master_t *spi,
                    SPI_t *module,
                    PORT_t *port,
					bool lsbFirst,
                    SPI_MODE_t mode,
                    SPI_INTLVL_t intLevel,
                    bool clk2x,
                    SPI_PRESCALER_t clockDivision);

void SPI_SlaveInit(SPI_Slave_t *spi,
                   SPI_t *module,
                   PORT_t *port,
                   bool lsbFirst,
                   SPI_MODE_t mode,
                   SPI_INTLVL_t intLevel);

void SPI_MasterCreateDataPacket(SPI_DataPacket_t *dataPacket,
                                const uint8_t *transmitData,
                                uint8_t *receiveData,
                                uint8_t bytesToTransceive,
                                PORT_t *ssPort,
                                uint8_t ssPinMask);

void SPI_MasterInterruptHandler(SPI_Master_t *spi);

uint8_t SPI_MasterInterruptTransceivePacket(SPI_Master_t *spi,
                                            SPI_DataPacket_t *dataPacket);

uint8_t SPI_MasterTransceiveByte(SPI_Master_t *spi, uint8_t TXdata);

bool SPI_MasterTransceivePacket(SPI_Master_t *spi,
                                SPI_DataPacket_t *dataPacket);

#endif
