/*
 * timer.c
 *
 *  Created on: 14.04.2013
 *      Author: Алексей
 */

#include "timer.h"

void SetupTimer( void )
{
	// Timer

	TA0CTL |= TASSEL_2 + MC_1;
	TA0CCTL0 |= CCIE;
	TA0CCR0 = 0x03E8; // 1 ms tick

	tick = 0;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void tim_interrupt( void )
{
	tick++;
	LPM0_EXIT;
}

