/*
 * led.c
 *
 *  Created on: 11.04.2013
 *      Author: Алексей
 */

#include "led.h"

void LED_Handler( Message msg, void* ptr, Handler* handler)
{
	switch(msg.what)
	{
	case LED_SWITCH_MSG:
		LED_POUT^=LED;
		Message* msg2send = Handler_obtain( handler, LED_SWITCH_MSG);
		Handler_sendMessageDelayed( handler, msg2send, 1000 );
		break;
//	case LED_OFF_MSG:
//		LED_POUT&=~LED;
//		Handler_sendEmptyMessage( handler, LED_ON_MSG );
//		break;
	}
}

