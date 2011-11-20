/*
 * ExampleHandler.c
 *
 *  Created on: Oct 26, 2011
 *      Author: Yuriy
 */

#include "ExampleHandler.h"
#include "handler.h"
#include "usart_driver_RTOS.h"
#include "spi_driver.h"
#include "leds.h"
#include "LedEventProcessorTask.h"
#include "strings.h"

void ExampleHandler_handleMessage(Message msg, void *handlerContext, Handler *handler) {
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	ExampleHandlerContext *context = (ExampleHandlerContext *)handlerContext;
	//store pointer to usart for convenience
	SpiDevice * master = context->spiMaster;
	LedsEventQueue *led = context->led;
	Usart *usart;
	uint8_t receivedChar='#';
	char obtainedMutex;

	switch (msg.what) {
	case EVENT_RUN_SPI_TEST:
		usart = msg.ptr;
		master = context->spiMaster;
		obtainedMutex = SpiMaster_startTransmission(master, 10);
		if (obtainedMutex) {
			//Transmit bytes
			Usart_putPgmString(usart, Strings_SpiMasterExample1, 10);
			receivedChar = SpiMaster_shiftByte(master, 0xC0);
			Usart_putInt(usart,receivedChar,16,10);
			receivedChar = SpiMaster_shiftByte(master, 0x01);
			Usart_putInt(usart,receivedChar,16,10);
			Usart_putPgmString(usart, Strings_newline, 10);
			//Transmit more bytes
			vTaskDelay(1);
			Usart_putPgmString(usart, Strings_SpiMasterExample2, 10);
			receivedChar = SpiMaster_shiftByte(master, 0xC0);
			Usart_putInt(usart,receivedChar,16,10);
			receivedChar = SpiMaster_shiftByte(master, 0xDE);
			Usart_putInt(usart,receivedChar,16,10);
			Usart_putPgmString(usart, Strings_newline, 10);
			//Transmit more bytes
			vTaskDelay(500);
			Usart_putPgmString(usart, Strings_SpiMasterExample3, 10);
			receivedChar = SpiMaster_shiftByte(master, 0xD0);
			Usart_putInt(usart,receivedChar,16,10);
			receivedChar = SpiMaster_shiftByte(master, 0x00);
			Usart_putInt(usart,receivedChar,16,10);
			receivedChar = SpiMaster_shiftByte(master, 0xDE);
			Usart_putInt(usart,receivedChar,16,10);
			Usart_putPgmString(usart, Strings_newline, 10);
			SpiMaster_stopTransmission(master);
		}
		break;

	case EVENT_BLINK:
		usart = msg.ptr;
		led = context->led;
		LedsEvent_put(led,0x01,100);
		LedsEvent_put(led,0x03,100);
		LedsEvent_put(led,0x06,100);
		LedsEvent_put(led,0x0c,100);
		LedsEvent_put(led,0x18,100);
		LedsEvent_put(led,0x30,100);
		LedsEvent_put(led,0x60,100);
		LedsEvent_put(led,0x40,100);
		Usart_putPgmString(usart, Strings_BlinkResp, 10);
		break;
	}
}
