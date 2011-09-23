/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA firmware for prototype SPI board source
 *
 *      This file contains example application
 *-----------------------------------------------------------------------------
 *      Naming conventions
 *      Code provided by Atmel is written in C convention, like this:
 *      CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
 *
 *      Code provided by Real Time Engineers (FreeRTOS.org) is
 *      provided in Hungarian convention using CamelCase:
 *      vTaskStartScheduler();
 *
 *      My code, since I  am more a Java guy, is in CamelCase.
 *      Although, giving that there are no classes, I cannot name
 *      functions starting with action, like we do in Java - thing.doSomeStuff()
 *      Instead, first comes comes the thing to which action is related:
 *      ledGroupEventQueuePut(ledRGBEventQueue,BLUE,700);
 *-------------------------------------------------------------------------------
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
#include "timers.h"
/* Atmel drivers */
#include "clksys_driver.h"
#include "pmic_driver.h"
#include "wdt_driver.h"
/* File headers. */

#include "led.h"
#include "ledGroup.h"

#include "spi_task.h"
#include "usart_task.h"
/** This is global, because used in hooks */
LedGroup * ledRGB;
/** BADISR_vect is called when interrupt has occurred, but there is no ISR handler for it defined */
ISR (BADISR_vect) {
	//stop execution and report error
	while(true) ledGroupSet(ledRGB, ORANGE);
}

/**
 * Callback function that will reset the WDT
 * @param xTimer
 */
void watchdogTimerCallback( xTimerHandle xTimer ) {
	WDT_Reset();
}

/**
 * Main is used to:
 * 	Initialize driver structures
 * 	Create tasks
 * 	Pass driver structures to tasks
 */
int main( void ) {
	/*
	 * Enable internal 32 MHz ring oscillator and wait until it's
	 * stable. Set the 32 MHz ring oscillator as the main clock source.
	 */
	CLKSYS_Enable( OSC_RC32MEN_bm );
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
	do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );

	//Enable watchdog timer, which will be reset by timer
	WDT_EnableAndSetTimeout( WDT_PER_1KCLK_gc );
	/*
	 * Do all configuration and create all tasks and queues before scheduler is started.
	 * It is possible to put initialization of peripherals like displays into task functions
	 * (which will be executed after scheduler has started) if fast startup is needed.
	 * Interrupts are not enabled until the call of vTaskStartScheduler();
	 */
	// Enable the Round-Robin Scheduling scheme.Round-Robin scheme ensures that no low-level
	// interrupts are “starved”, as the priority changes continuously
	PMIC_EnableRoundRobin();

	//Create and start the timer, which will reset Watch Dog Timer
	xTimerStart(xTimerCreate((signed char*)"WDT",500, pdTRUE, 0, watchdogTimerCallback), 0);
	//---------Use USART on PORTC----------------------------
	Usart * usartFTDI = Usart_initialize(&USARTE0, BAUD9600, 128);
	//---------Start LED task for testing purposes-----------
	ledRGB = ledGroupInitialize(3);
	ledGroupAdd(ledRGB, &PORTF, 0x04,1 );//R
	ledGroupAdd(ledRGB, &PORTF, 0x08,1 );//G
	ledGroupAdd(ledRGB, &PORTF, 0x02,1 );//B
	ledGroupSet(ledRGB, BLUE);
	LedGroupEventQueue * ledRGBEventQueue = startLedQueueProcessorTask(ledRGB,configLOW_PRIORITY, NULL);
	startBlinkingLedTask(ledRGBEventQueue,configLOW_PRIORITY, NULL);

	LedGroup *ledString = ledGroupInitialize(7);
	ledGroupAdd(ledString, &PORTA, 0x02, 0);
	ledGroupAdd(ledString, &PORTA, 0x04, 0);
	ledGroupAdd(ledString, &PORTA, 0x08, 0);
	ledGroupAdd(ledString, &PORTA, 0x10, 0);
	ledGroupAdd(ledString, &PORTA, 0x20, 0);
	ledGroupAdd(ledString, &PORTA, 0x40, 0);
	ledGroupAdd(ledString, &PORTA, 0x80, 0);
	LedGroupEventQueue *ledStringQueue = startLedQueueProcessorTask(ledString, configLOW_PRIORITY, NULL);

	// Start USART task
	startUsartTask(usartFTDI, ledRGBEventQueue, 128, configNORMAL_PRIORITY, NULL);

	// Initialize SPI slave on port D
	SpiSlave * spiSlaveD = SpiSlave_init(&SPID,false,SPI_MODE_0_gc,64);
	startSpiSlaveTask(spiSlaveD, usartFTDI, configLOW_PRIORITY, NULL);

	// Initialize SPI master on port C
	SpiMaster * spiMasterC = SpiMaster_init(&SPIC, false, SPI_MODE_0_gc, false, SPI_PRESCALER_DIV4_gc);
	SpiDevice * spiMasterCdefault = SpiMaster_initDevice(spiMasterC, &PORTC, SPI_SS_bm);
	startSpiMasterTask(spiMasterCdefault, usartFTDI,configLOW_PRIORITY, NULL);

	/* Start scheduler. Creates idle task and returns if failed to create it.
	 * vTaskStartScheduler never returns during normal operation. It is unlikely that XMEGA port will need to
	 * dynamically create tasks or queues. To ensure stable work, create ALL tasks and ALL queues before
	 * vTaskStartScheduler call.
	 * Interrupts would be enabled by calling PMIC_EnableLowLevel();*/
	vTaskStartScheduler();

	/* Should never get here, stop execution and report error */
	while(true) ledGroupSet(ledRGB, PINK);
	return 0;
}

/**
 * This function is only called when there are no tasks to execute, except for the idle task. Most commonly it
 * is used to schedule co-routines or do some unimportant jobs. It is also great to put sleep(); to save
 * power. Microcontroller will stop code execution until the next interrupt from tick timer or peripheral.
 * configUSE_IDLE_HOOK should be defined as 1 to use IdleHook
 */
void vApplicationIdleHook( void ) {
	// Go to sleep mode if there are no active tasks
	sleep_mode();
}

/**
 * This function is called immediately after task context is saved into stack. This is the case
 * when stack contains biggest amount of data. Hook function checks if there is a stack overflow
 * for the current (switched) task. Parameters could be used for debug output.
 * configCHECK_FOR_STACK_OVERFLOW should be defined as 1 to use StackOverflowHook.
 */
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {
	while(true) ledGroupSet(ledRGB, RED);
}

/**
 * There is not enough space in heap to allocate memory, which means that all heap space is
 * occupied by previous tasks and queues. Try to increase heap size configTOTAL_HEAP_SIZE in FreeRTOSConfig.h
 * XMEGA port uses heap_1.c which doesn't support memory free.
 * configUSE_MALLOC_FAILED_HOOK should be defined as 1 to use vApplicationMallocFailedHook()
 */
void vApplicationMallocFailedHook() {
	while(true) ledGroupSet(ledRGB, PINK);
}
