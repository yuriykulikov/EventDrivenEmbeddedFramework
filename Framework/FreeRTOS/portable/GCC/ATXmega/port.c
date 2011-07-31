/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA port source for FreeRTOS 
 *
 *      This file contains definitions and routines requred for FreeRTOS.
 *		This port uses XMega Real Time Clock to provide constand task switching
 *		frequency, regardless of main COU frequency changes.
 *		If RTC is used in application, use any other GP timer for RTOS needs.
 *		This file is based on port.c for ATmega323,
 *		Copyright (C) 2010 Real Time Engineers Ltd.
 *		Original file was modified to suit XMega. Changed counter interrupt
 *		source. Routines portSAVE_CONTEXT(), portRESTORE_CONTEXT()
 *		and *pxPortInitialiseStack() were modified to work in extended progam
 *		memory space.
 *		
 * \par Documentation
 *		http://www.FreeRTOS.org - Documentation, latest information, license and
 *		contact details.
 *		
 * \author
 *      Universität Erlangen-Nürnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 *
 *****************************************************************************/


/* Compiler definitions include file. */
#include "avr_compiler.h"
#include "FreeRTOSConfig.h"
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#include "TC_driver.h"
#include "pmic_driver.h"

/* define this as 1 if target device more than 128K of program memory.
 * define as 0 if 128K or less. Devices with more memory have 24-bit program
 * counter, while devices with 128K or less have only 16-bit program counter.
 */
#if defined (__AVR_ATxmega16A4__)
	#define PROGRAMCOUNTER_24BIT 0
#elif defined (__AVR_ATxmega16D4__)
	#define PROGRAMCOUNTER_24BIT 0
#elif defined (__AVR_ATxmega32A4__)
	#define PROGRAMCOUNTER_24BIT 0
#elif defined (__AVR_ATxmega32D4__)
	#define PROGRAMCOUNTER_24BIT 0
#elif defined (__AVR_ATxmega64A1__)
	#define PROGRAMCOUNTER_24BIT 0
#elif defined (__AVR_ATxmega64A3__)
	#define PROGRAMCOUNTER_24BIT 0
#elif defined (__AVR_ATxmega64D3__)
	#define PROGRAMCOUNTER_24BIT 0
#elif defined (__AVR_ATxmega128A1__)
	#define PROGRAMCOUNTER_24BIT 1
#elif defined (__AVR_ATxmega128A3__)
	#define PROGRAMCOUNTER_24BIT 1
#elif defined (__AVR_ATxmega128D3__)
	#define PROGRAMCOUNTER_24BIT 1
#elif defined (__AVR_ATxmega192A3__)
	#define PROGRAMCOUNTER_24BIT 1
#elif defined (__AVR_ATxmega192D3__)
	#define PROGRAMCOUNTER_24BIT 1
#elif defined (__AVR_ATxmega256A3__)
	#define PROGRAMCOUNTER_24BIT 1
#elif defined (__AVR_ATxmega256A3B__)
	#define PROGRAMCOUNTER_24BIT 1
#elif defined (__AVR_ATxmega256D3__)
#else
	#error "device name should be defined in the project settings"
#endif

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the AVR XMEGA port.
 *----------------------------------------------------------*/

/* Start tasks with interrupts enables. */
#define portFLAGS_INT_ENABLED					( ( portSTACK_TYPE ) 0x80 )

/*-----------------------------------------------------------*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;

/*-----------------------------------------------------------*/


/* 
 * Macro to save all the general purpose registers, the save the stack pointer
 * into the TCB.  
 * 
 * The first thing we do is save the flags then disable interrupts.  This is to 
 * guard our stack against having a context switch interrupt after we have already 
 * pushed the registers onto the stack - causing the 32 registers to be on the 
 * stack twice. 
 * 
 * r1 is set to zero as the compiler expects it to be thus, however some
 * of the math routines make use of R1. 
 * 
 * The interrupts will have been disabled during the call to portSAVE_CONTEXT()
 * so we need not worry about reading/writing to the stack pointer. 
 */

#define portSAVE_CONTEXT()									\
	asm volatile (	"push	r0						\n\t"	\
					"in		r0, __SREG__			\n\t"	\
					"cli							\n\t"	\
					"push	r0						\n\t"	\
					"push	r1						\n\t"	\
					"clr	r1						\n\t"	\
					"push	r2						\n\t"	\
					"push	r3						\n\t"	\
					"push	r4						\n\t"	\
					"push	r5						\n\t"	\
					"push	r6						\n\t"	\
					"push	r7						\n\t"	\
					"push	r8						\n\t"	\
					"push	r9						\n\t"	\
					"push	r10						\n\t"	\
					"push	r11						\n\t"	\
					"push	r12						\n\t"	\
					"push	r13						\n\t"	\
					"push	r14						\n\t"	\
					"push	r15						\n\t"	\
					"push	r16						\n\t"	\
					"push	r17						\n\t"	\
					"push	r18						\n\t"	\
					"push	r19						\n\t"	\
					"push	r20						\n\t"	\
					"push	r21						\n\t"	\
					"push	r22						\n\t"	\
					"push	r23						\n\t"	\
					"push	r24						\n\t"	\
					"push	r25						\n\t"	\
					"push	r26						\n\t"	\
					"push	r27						\n\t"	\
					"push	r28						\n\t"	\
					"push	r29						\n\t"	\
					"push	r30						\n\t"	\
					"push	r31						\n\t"	\
					";in	r16, 0x38				\n\t"	\
					";push	r16						\n\t"	\
					";in	r16, 0x39				\n\t"	\
					";push	r16						\n\t"	\
					";in	r16, 0x3a				\n\t"	\
					";push	r16						\n\t"	\
					";in	r16, 0x3b				\n\t"	\
					";push	r16						\n\t"	\
					";in	r16, 0x3c				\n\t"	\
					";push	r16						\n\t"	\
					"lds	r26, pxCurrentTCB		\n\t"	\
					"lds	r27, pxCurrentTCB + 1	\n\t"	\
					"in		r0, 0x3d				\n\t"	\
					"st		x+, r0					\n\t"	\
					"in		r0, 0x3e				\n\t"	\
					"st		x+, r0					\n\t"	\
				);

/* 
 * Opposite to portSAVE_CONTEXT().  Interrupts will have been disabled during
 * the context save so we can write to the stack pointer. 
 */

#define portRESTORE_CONTEXT()								\
	asm volatile (	"lds	r26, pxCurrentTCB		\n\t"	\
					"lds	r27, pxCurrentTCB + 1	\n\t"	\
					"ld		r28, x+					\n\t"	\
					"out	__SP_L__, r28			\n\t"	\
					"ld		r29, x+					\n\t"	\
					"out	__SP_H__, r29			\n\t"	\
					";pop	r16						\n\t"	\
					";out	0x3c, r16				\n\t"	\
					";pop	r16						\n\t"	\
					";out	0x3b, r16				\n\t"	\
					";pop	r16						\n\t"	\
					";out	0x3a, r16				\n\t"	\
					";pop	r16						\n\t"	\
					";out	0x39, r16				\n\t"	\
					";pop	r16						\n\t"	\
					";out	0x38, r16				\n\t"	\
					"pop	r31						\n\t"	\
					"pop	r30						\n\t"	\
					"pop	r29						\n\t"	\
					"pop	r28						\n\t"	\
					"pop	r27						\n\t"	\
					"pop	r26						\n\t"	\
					"pop	r25						\n\t"	\
					"pop	r24						\n\t"	\
					"pop	r23						\n\t"	\
					"pop	r22						\n\t"	\
					"pop	r21						\n\t"	\
					"pop	r20						\n\t"	\
					"pop	r19						\n\t"	\
					"pop	r18						\n\t"	\
					"pop	r17						\n\t"	\
					"pop	r16						\n\t"	\
					"pop	r15						\n\t"	\
					"pop	r14						\n\t"	\
					"pop	r13						\n\t"	\
					"pop	r12						\n\t"	\
					"pop	r11						\n\t"	\
					"pop	r10						\n\t"	\
					"pop	r9						\n\t"	\
					"pop	r8						\n\t"	\
					"pop	r7						\n\t"	\
					"pop	r6						\n\t"	\
					"pop	r5						\n\t"	\
					"pop	r4						\n\t"	\
					"pop	r3						\n\t"	\
					"pop	r2						\n\t"	\
					"pop	r1						\n\t"	\
					"pop	r0						\n\t"	\
					"out	__SREG__, r0			\n\t"	\
					"pop	r0						\n\t"	\
				);

/*-----------------------------------------------------------*/

/*
 * Perform hardware setup to enable ticks from timer 1, compare match A.
 */
static void prvSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

/* 
 * See header file for description. 
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
	/*The addresses are 16 or 24 bit depending on the xmega memory, so use 32 bit variable but put only a
	 * part to stack.*/
	uint32_t usAddress;
	/* Place a few bytes of known values on the bottom of the stack. 
	This is just useful for debugging. */
	*pxTopOfStack = 0x11;
	pxTopOfStack--;
	*pxTopOfStack = 0x22;
	pxTopOfStack--;
	*pxTopOfStack = 0x33;
	pxTopOfStack--;
	/* Simulate how the stack would look after a call to vPortYield() generated by 
	the compiler. */
	/*lint -e950 -e611 -e923 Lint doesn't like this much - but nothing I can do about it. */
	/* The start of the task code will be popped off the stack last, so place
	it on first. */
	/*	Original code
	 * For 16-bit program counter (128K program memory or less)
	usAddress = ( unsigned short ) pxCode;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned short ) 0x00ff );
	pxTopOfStack--;
	usAddress >>= 8;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned short ) 0x00ff );
	pxTopOfStack--;*/
	/* end of original code block */

	/* The way it should be done for xmega with probably  more than 128K program memory.
	 * Warning is OK here - type incompatibility does not matter - usAddress is only
	 * used as temporary storage */
	usAddress = ( uint32_t ) pxCode;

	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( uint32_t ) 0x000000ff );
	pxTopOfStack--;
	usAddress >>= 8;

	/* The only difference between ports for different xmegas is size of
	 * program counter. 16-bit for devices with 128K of program memory or less.
	 * 24-bit for other xmegas. */
#if PROGRAMCOUNTER_24BIT == 1
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( uint32_t ) 0x000000ff );
	pxTopOfStack--;
	usAddress >>= 8;
#endif

	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( uint32_t ) 0x000000ff );
	pxTopOfStack--;


	/* Next simulate the stack as if after a call to portSAVE_CONTEXT().  
	portSAVE_CONTEXT places the flags on the stack immediately after r0
	to ensure the interrupts get disabled as soon as possible, and so ensuring
	the stack use is minimal should a context switch interrupt occur. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	/* R0 */
	pxTopOfStack--;
	*pxTopOfStack = portFLAGS_INT_ENABLED;
	pxTopOfStack--;


	/* Now the remaining registers.   The compiler expects R1 to be 0. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x02;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04;	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x05;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x13;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x14;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x15;	/* R15 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x16;	/* R16 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x17;	/* R17 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x18;	/* R18 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x19;	/* R19 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x20;	/* R20 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x21;	/* R21 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x22;	/* R22 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x23;	/* R23 */
	pxTopOfStack--;

	/* Place the parameter on the stack in the expected location. */
	usAddress = ( unsigned short ) pvParameters;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned short ) 0x00ff );
	pxTopOfStack--;

	usAddress >>= 8;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned short ) 0x00ff );
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0x26;	/* R26 X */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x27;	/* R27 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x28;	/* R28 Y */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x29;	/* R29 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x30;	/* R30 Z */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x31;	/* R31 */
	pxTopOfStack--;
	/* these registers are also part of execution context
	 * Corresponing registars are saved and restored in saveCONTEXT and restoreCONTEXT
	 * TODO  See datasheet for explanation
	 */
	//*pxTopOfStack = ( portSTACK_TYPE ) 0x38;	/* 38 RAMPD */
	//pxTopOfStack--;
	//*pxTopOfStack = ( portSTACK_TYPE ) 0x39;	/* 39 RAMPX */
	//pxTopOfStack--;
	//*pxTopOfStack = ( portSTACK_TYPE ) 0x3a;	/* 3a RAMPY */
	//pxTopOfStack--;
	//*pxTopOfStack = ( portSTACK_TYPE ) 0x3b;	/* 3b RAMPZ */
	//pxTopOfStack--;
	//*pxTopOfStack = ( portSTACK_TYPE ) 0x3c;	/* 3c EIND */
	//pxTopOfStack--;


	/*lint +e950 +e611 +e923 */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/
portBASE_TYPE xPortStartScheduler( void )
{

	/* Setup the hardware to generate the tick. */
	prvSetupTimerInterrupt();


	/* Restore the context of the first task that is going to run. */
	portRESTORE_CONTEXT();

	/* Simulate a function call end as generated by the compiler.  We will now
	jump to the start of the task the context of which we have just restored. */
	asm volatile ( "ret" );


	/* Should not get here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the AVR port will get stopped.  If required simply
	disable the tick interrupt here. */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch.  The first thing we do is save the registers so we
 * can use a naked attribute.
 */
void vPortYield( void ) __attribute__ ( ( naked ) );
void vPortYield( void )
{
	portSAVE_CONTEXT();
	vTaskSwitchContext();
	portRESTORE_CONTEXT();

	asm volatile ( "ret" );
}
/*-----------------------------------------------------------*/

/*
 * Context switch function used by the tick.  This must be identical to 
 * vPortYield() from the call to vTaskSwitchContext() onwards.  The only
 * difference from vPortYield() is the tick count is incremented as the
 * call comes from the tick ISR.
 */
void vPortYieldFromTick( void ) __attribute__ ( ( naked ) );
void vPortYieldFromTick( void )
{
	unsigned portBASE_TYPE uxSavedStatReg;
	portSAVE_CONTEXT();
	uxSavedStatReg = portSET_INTERRUPT_MASK_FROM_ISR();
	vTaskIncrementTick();
	vTaskSwitchContext();
	portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedStatReg);
	portRESTORE_CONTEXT();

	asm volatile ( "reti" );
}
/*-----------------------------------------------------------*/

/*
 * Setup timer 1 compare match A to generate a tick interrupt.
 */
static void prvSetupTimerInterrupt( void )
{
	//Use TCC0 as a tick counter. If this is to be changed, change ISR as well
	TC0_t * tickTimer = &TCC0;
	//select the clock source and pre-scale by 64
	TC0_ConfigClockSource(tickTimer,TC_CLKSEL_DIV64_gc);
	//set period of counter
	tickTimer->PER = configCPU_CLOCK_HZ / configTICK_RATE_HZ/64-1;

	//enable interrupt and set level
#if configKERNEL_INTERRUPT_PRIORITY==1
	TC0_SetOverflowIntLevel(tickTimer,TC_OVFINTLVL_LO_gc);
#elif configKERNEL_INTERRUPT_PRIORITY==2
	TC0_SetOverflowIntLevel(tickTimer,TC_OVFINTLVL_MED_gc);
#elif configKERNEL_INTERRUPT_PRIORITY==3
	TC0_SetOverflowIntLevel(tickTimer,TC_OVFINTLVL_HI_gc);
#endif


	//enable all interrupt levels. Some levels will be disabled when entering a critical section
	PMIC_EnableLowLevel();
	PMIC_EnableMediumLevel();
	PMIC_EnableHighLevel();
}
/*-----------------------------------------------------------*/

unsigned portBASE_TYPE uxPortSetInterruptMaskFromISR()
{
	// first save interrupt control register
	unsigned portBASE_TYPE uxSavedStatReg = PMIC.CTRL;
	unsigned portBASE_TYPE uxNewStatReg = 0;
// Prober bits to be set for configMAX_SYSCALL_INTERRUPT_PRIORITY are selected in compile-time
// It will allow execution of interrupts with the level higher than configMAX_SYSCALL_INTERRUPT_PRIORITY
// within a critical section
#ifndef configMAX_SYSCALL_INTERRUPT_PRIORITY
	#error "configMAX_SYSCALL_INTERRUPT_PRIORITY is not defined"
	#endif
#if configMAX_SYSCALL_INTERRUPT_PRIORITY==3
	uxNewStatReg |= PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY==2
	uxNewStatReg |= PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY==1
	uxNewStatReg |= PMIC_LOLVLEN_bm;
#endif
	// Now clear these bits in the interrupt control register
	PMIC.CTRL &= ~(uxNewStatReg);
	// Return saved value to be restored after the context switch
	return uxSavedStatReg;
}


void vPortClearInterruptMaskFromISR( unsigned portBASE_TYPE uxSavedStatReg)
{
	PMIC.CTRL = uxSavedStatReg;
}

#if configUSE_PREEMPTION == 1

	/*
	 * Tick ISR for preemptive scheduler.  We can use a naked attribute as
	 * the context is saved at the start of vPortYieldFromTick().  The tick
	 * count is incremented after the context is saved.
	 */

	ISR (TCC0_OVF_vect, ISR_NAKED)
	{
		vPortYieldFromTick();
		asm volatile ( "reti" );
	}

#else

	/*
	 * Tick ISR for the cooperative scheduler.  All this does is increment the
	 * tick count.  We don't need to switch context, this can only be done by
	 * manual calls to taskYIELD();
	 */
	ISR (TCC0_OVF_vect, ISR_NAKED)
	{
		vTaskIncrementTick();
	}
#endif
	
