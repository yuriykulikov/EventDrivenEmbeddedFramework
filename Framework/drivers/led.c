/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA board-specific functions and tasks source.
 *
 *      This file contains an application that connects cell phone via SPI
 *     
 * \author
 *      Universität Erlangen-Nürnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 * \par Documentation
 *     http://www.FreeRTOS.org - Documentation, latest information.
 *
 *****************************************************************************/

/* Compiler definitions include file. */
#include "avr_compiler.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "semphr.h"
/* File headers. */
#include "led.h"
#include "port_driver.h"


/*
 * Low level simple non-RTOS function
 */
void LED_set(Color_enum Color)
{
	PORTLED.OUTSET=LED_gm;
	PORTLED.OUTTGL=Color;
}
/*
 * Low level simple non-RTOS function
 */
void LED_toggle(Color_enum Color)
{
	PORTLED.OUTTGL=Color;
}
/*-----------------------------------------------------------*/
void LED_queue_put (xQueueHandle queue,Color_enum Color, uint16_t Duration)
{
	//Create new empty LED structure
	LED_event_struct_t newLedEvent;
	//Set value according to parameters
	newLedEvent.Color=Color;
	newLedEvent.Duration=Duration;
	/* Send  event data to queue. After this LEDTask will be unblocked as soon as
	 * all higher priority tasks get blocked, delayed, suspended - whatever.*/
	xQueueSendToBack(queue,&newLedEvent,0);
}

void vDebugLedTask( void *pvParameters )
{
	xQueueHandle ledQueueHandle = (xQueueHandle) pvParameters;
	/* Cycle for ever*/
	while(true)
	{
		LED_event_struct_t newLedEvent;
		//Check while there are new events on the queue. Wait for debugLed.ConstantDelay time each time
		while(xQueueReceive(ledQueueHandle, &newLedEvent, portMAX_DELAY ) == pdPASS )
		{
			//TODO grab mutex
			LED_set(newLedEvent.Color);
			vTaskDelay(newLedEvent.Duration);
			LED_set(NONE);
		}
		//TODO release mutex
	}
}

/* Creates a queue and spawns a task */
xQueueHandle startDebugLedTask (char cPriority)
{

	/* This configures pins 0,1,2 on PORTC for totempole and pull-up, no slew rate and no invertion. */
	PORT_ConfigurePins( &PORTLED,LED_gm,false,false,PORT_OPC_PULLUP_gc,PORT_ISC_BOTHEDGES_gc );
	/* Configure PORTLED pins outputs. */
	PORT_SetDirection( &PORTLED, LED_gm );

	/* Create Queue for LED events */
	xQueueHandle xQueueLED=xQueueCreate(8, sizeof(LED_event_struct_t));
	/* Spawn task. */
	xTaskCreate(vDebugLedTask, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, xQueueLED, cPriority, NULL );
	return xQueueLED;
};

void BlinkingLedTask( void *pvParameters )
{
	xQueueHandle ledQueueHandle = (xQueueHandle) pvParameters;
	while(true)
	{
		if (xQueueIsQueueEmptyFromISR(ledQueueHandle)){
			LED_queue_put(ledQueueHandle,GREEN,500);
		}
		vTaskDelay(1000);
	}
}
