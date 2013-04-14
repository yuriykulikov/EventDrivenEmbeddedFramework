/*
 * hal.c
 *
 *  Created on: 11.04.2013
 *      Author: Алексей
 */

#include "hal.h"

void InitHardware( void )
{
	// Clock

	//CSCTL3 |= DIVS__32;

	// PIO

	PJDIR |= BIT0;
}
