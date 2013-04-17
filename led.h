/*
 * led.h
 *
 *  Created on: 11.04.2013
 *      Author: Алексей
 */

#ifndef LED_H_
#define LED_H_

#include "hal/hal.h"

#include "Framework/Message.h"
#include "Framework/Handler.h"
#include "Framework/MsgQueue.h"

#define LED_POUT	PJOUT
#define LED	BIT0

#define LED_SWITCH_MSG	1
//#define LED_OFF_MSG	2

void LED_Handler(Message msg, void* ptr, Handler* handler);

#endif /* LED_H_ */
