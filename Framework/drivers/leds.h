/*
 * ledGroup.h
 *
 *  Created on: 12.06.2011
 *      Author: Yuriy
 */

#ifndef LEDGROUP_H_
#define LEDGROUP_H_

#include "avr_compiler.h"
/* One single pin */
typedef struct PinDefenition
{
	PORT_t * port;
	uint8_t bitmask;
} Pin;
/* Group of leds */
typedef struct LedGroupDefenition
{
	short amountOfLedsInGroup;
	Pin * pins;
} LedGroup;

/*
 * Assume that RGB led is a led group of 3 leds,
 * then colors are bitmasks.
 */
typedef enum {
	RED = 0x01,
	GREEN = 0x02,
    BLUE= 0x04,

    ORANGE = 0x03,
    PINK = 0x05,
    SKY = 0x06,

    WHITE = 0x07,
	NONE = 0x00,
} Color_enum;


LedGroup * Leds_init (short amountOfLedsInGroup);
short Leds_new(LedGroup * ledGroup, PORT_t * port, uint8_t bitmask, short isActiveLow);
void Leds_set(LedGroup * ledGroup , uint8_t bitmask);

#endif /* LEDGROUP_H_ */
