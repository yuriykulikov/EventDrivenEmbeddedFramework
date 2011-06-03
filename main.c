/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA firmware for prototype SPI board source
 *
 *      This file contains example application
 *     
 * \author
 *      Universität Erlangen-Nurnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *****************************************************************************/

/* Compiler definitions include file. */
#include "avr_compiler.h"
#include <avr/sleep.h>
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
/* Atmel drivers */
#include "clksys_driver.h"
#include "pmic_driver.h"
/* File headers. */

#include "led.h"
#include "spispy_task.h"
#include "usart_task.h"

/* BADISR_vect is called when interrupt has occurred, but there is no ISR handler for it defined */
ISR (BADISR_vect){
	//stop execution and report error
	while(true) LED_set(ORANGE);
}

int main( void )
{
	/*  Enable internal 32 MHz ring oscillator and wait until it's
	 *  stable. Set the 32 MHz ring oscillator as the main clock source.
	 */
	CLKSYS_Enable( OSC_RC32MEN_bm );
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
	do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
	/* Do all configuration and create all tasks and queues before scheduler is started.
	 * It is possible to put initialization of peripherals like displays into task functions
	 * (which will be executed after scheduler has started) if fast startup is needed.
	 * Interrupts are not enabled until the call of vTaskStartScheduler();
	 */


	USART_buffer_struct_t FTDI_USART;
	FTDI_USART = USART_InterruptDriver_Initialize(&USARTC0, BAUD9600, 64);
	/* Report itself. */
	USART_Buffer_PutString(&FTDI_USART, "XMEGA ready",DONT_BLOCK);
	/* Start USART task */
	xTaskCreate(vUSARTTask, ( signed char * ) "USARTTSK", 1000,&FTDI_USART, configNORMAL_PRIORITY, NULL );
	/* Start LED task for testing purposes */
	xQueueHandle debugLed = startDebugLedTask(configLOW_PRIORITY);
	xTaskCreate(BlinkingLedTask, ( signed char * ) "BLINK", configMINIMAL_STACK_SIZE, debugLed, configLOW_PRIORITY, NULL );
	/* Start SPISPY task */
	//vStartSPISPYTask(configNORMAL_PRIORITY);

	LED_queue_put(debugLed,BLUE,700);
	LED_queue_put(debugLed,SKY,700);
	LED_queue_put(debugLed,WHITE,700);
	/* Enable PMIC interrupt level low. */


	PMIC_EnableLowLevel();
	/* Start scheduler. Creates idle task and returns if failed to create it.
	 * vTaskStartScheduler never returns during normal operation. If it has returned, probably there is
	 * not enough space in heap to allocate memory for the idle task, which means that all heap space is
	 * occupied by previous tasks and queues. Try to increase heap size configTOTAL_HEAP_SIZE in FreeRTOSConfig.h
	 * XMEGA port uses heap_1.c which doesn't support memory free. It is unlikely that XMEGA port will need to
	 * dynamically create tasks or queues. To ensure stable work, create ALL tasks and ALL queues before
	 * vTaskStartScheduler call. In this case we can be sure that heap size is enough.
	 * Interrupts would be enabled by calling sei();*/
	vTaskStartScheduler();

	/* stop execution and report error */
	LED_set(PINK);
	while(true) LED_set(PINK);
	return 0;
}
/* Prototype */
void vApplicationIdleHook( void );
/* This function is only called when there are no tasks to execute, except for the idle task. Most commonly it
 * is used to schedule co-routines or do some unimportant jobs. It is also great to put sleep(); to save
 * power. Microcontroller will stop code execution until the next interrupt from tick timer or peripheral.
 * configUSE_IDLE_HOOK should be defined as 1 to use IdleHook
 */
void vApplicationIdleHook( void )
{
   /* Go to sleep mode if there are no active tasks	*/
	sleep_mode();
}
/* Prototype */
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
	/* This function is called immediately after task context is saved into stack. This is the case
	 * when stack contains biggest amount of data. Hook function checks if there is a stack overflow
	 * for the current (switched) task. Parameters could be used for debug output.
	 * configCHECK_FOR_STACK_OVERFLOW should be defined as 1 to use StackOverflowHook.
	 */
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	/* stop execution and report error */
	while(true) LED_set(RED);
}
void vApplicationTickHook( void );
/* This function is called during the tick interrupt. configUSE_TICK_HOOK should be defined as 1.*/
void vApplicationTickHook( void )
{
/* Tick hook could be used to implement timer functionality*/
}
