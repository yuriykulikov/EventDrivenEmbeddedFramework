/*
 * WorkerTask.c
 *
 *  Created on: Oct 26, 2011
 *      Author: Yuriy
 */

#include "LooperTask.h"
#include "CommandInterpreter.h"
#include "handler.h"
#include "usart_driver_RTOS.h"
#include "spi_driver.h"
#include "ledGroup.h"
#include "strings.h"

void LooperTask( void *pvParameters )
{
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	LooperTaskParameters * parameters = (LooperTaskParameters *)pvParameters;
	//store pointer to usart for convenience
	SpiDevice * master = parameters->spiMaster;
	LedGroupEventQueue *led = parameters->led;

	//Infinite loop
	for (;;) {
		Message msg;
		uint8_t receivedChar='#';
		char obtainedMutex;
		Usart *usart;
		if (xQueueReceive(parameters->handler->taskQueue, &msg, portMAX_DELAY)) {
			switch (msg.what) {
			case EVENT_RUN_SPI_TEST:
				usart = msg.ptr;
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
				ledGroupEventQueuePut(led,0x01,100);
				ledGroupEventQueuePut(led,0x03,100);
				ledGroupEventQueuePut(led,0x06,100);
				ledGroupEventQueuePut(led,0x0c,100);
				ledGroupEventQueuePut(led,0x18,100);
				ledGroupEventQueuePut(led,0x30,100);
				ledGroupEventQueuePut(led,0x60,100);
				ledGroupEventQueuePut(led,0x40,100);
				Usart_putPgmString(usart, Strings_BlinkResp, 10);
				break;
			}
		}
	}
}

void startLooperTask(Handler *handler,
		CommandLineInterpreter *interpreter,
		SpiDevice *master,
		Usart *usart,
		LedGroupEventQueue *led,
		char cPriority, xTaskHandle taskHandle)
{

	//Task will receive these parameters later, they should be either allocated dynamically or be static
	LooperTaskParameters * looperTaskParameters = pvPortMalloc(sizeof(LooperTaskParameters));
	looperTaskParameters->spiMaster = master;
	looperTaskParameters->usart = usart;
	looperTaskParameters->led = led;
	looperTaskParameters->handler = handler;
	// Spawn task, stack size is approximate, seems to work well
	xTaskCreate(LooperTask, ( signed char * ) "LOOPER", 350, looperTaskParameters, cPriority, NULL );
}
