#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Compiler definitions include file. */
#include "avr_compiler.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/
//number of priorities include configIDLE_PRIORITY = 0
#define configLOW_PRIORITY 1
#define configNORMAL_PRIORITY 2
#define configHIGH_PRIORITY 3

#define configUSE_PREEMPTION		1 //1
#define configUSE_IDLE_HOOK			1 //1
#define configUSE_TICK_HOOK			0 //0
#define configCPU_CLOCK_HZ			( ( unsigned long ) F_CPU )//2MHz is default value for xmega.
//If you you want another frequency don't forget to modify period of timer counter used for tick interrupt
#define configTICK_RATE_HZ			( ( portTickType ) 1000 )
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 4 )//4
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 150 )//85
#define configTOTAL_HEAP_SIZE		( (size_t ) ( 3500 ) )//1500
#define configMAX_TASK_NAME_LEN		( 8 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		1
#define configIDLE_SHOULD_YIELD		1
#define configQUEUE_REGISTRY_SIZE	0

#define configUSE_MUTEXES			1

// timers.c
#define configUSE_TIMERS				1
#define configTIMER_TASK_PRIORITY		configLOW_PRIORITY
#define configTIMER_QUEUE_LENGTH		16
#define configTIMER_TASK_STACK_DEPTH	configMINIMAL_STACK_SIZE

/* Debug */
#define configCHECK_FOR_STACK_OVERFLOW			1//0
#define configGENERATE_RUN_TIME_STATS			0//0
#define portGET_RUN_TIME_COUNTER_VALUE			0//0
#define INCLUDE_uxTaskGetStackHighWaterMark		1
#define INCLUDE_xTaskGetCurrentTaskHandle 		1
#define configUSE_MALLOC_FAILED_HOOK			1//0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0 //1
#define configMAX_CO_ROUTINE_PRIORITIES ( 0 )//2

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		0
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				0//1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			0
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1


#endif /* FREERTOS_CONFIG_H */
