/*
 * hal.h
 *
 *  Created on: 11.04.2013
 *      Author: Алексей
 */

#ifndef HAL_H_
#define HAL_H_

#include <msp430.h>

#define DISABLE_INTERRUPTS	_disable_interrupts()
#define ENABLE_INTERRUPTS	_enable_interrupts()

void InitHardware( void );

#endif /* HAL_H_ */
