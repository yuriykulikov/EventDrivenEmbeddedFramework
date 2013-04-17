#include "hal/hal.h"
#include "hal/timer.h"
#include "Framework/Message.h"
#include "Framework/Handler.h"
#include "Framework/MsgQueue.h"
#include "led.h"

/*
 * main.c
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    InitHardware();
    SetupTimer();

    MsgQueue mainQueue;
    MsgQueue_init( &mainQueue, 0 );

    Handler test_handler;
    Handler_init( &test_handler, &mainQueue, LED_Handler, NULL_CONTEXT );
    Handler_sendEmptyMessage( &test_handler, LED_SWITCH_MSG );

    ENABLE_INTERRUPTS;

    while(1)
    {
    	MsgQueue_processNextMessage( &mainQueue );
    	LPM0;
    }

	return 0;
}
