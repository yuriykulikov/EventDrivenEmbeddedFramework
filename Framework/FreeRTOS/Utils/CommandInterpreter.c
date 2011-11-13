/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/* Standard includes. */
#include <string.h>
#include <avr/pgmspace.h>
#include "strings.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Utils includes. */
#include "CommandInterpreter.h"
#include "handler.h"
#include "usart_driver_RTOS.h"

CommandLineInterpreter * CommandLineInterpreter_create() {
	CommandLineInterpreter *commandLineInterpreter = (CommandLineInterpreter*) pvPortMalloc(sizeof(CommandLineInterpreter));
	commandLineInterpreter->list =  (xCommandLineInputListItem*)pvPortMalloc(sizeof(xCommandLineInputListItem));
	commandLineInterpreter->list->pxNext = NULL;
	return commandLineInterpreter;
}

/*-----------------------------------------------------------*/
void CommandLineInterpreter_register(CommandLineInterpreter *interpreter, char *pgm_Cmd, char *pgm_CmdDesc, Handler *handler, char what) {
	/* Create a new list item that will reference the command being registered. */
	xCommandLineInputListItem *pxNewListItem = (xCommandLineInputListItem*) pvPortMalloc(sizeof(xCommandLineInputListItem));
	// this will be the last element
	pxNewListItem->pxNext = NULL;
	// now populate the entry from input parameters
	pxNewListItem->pcCommand = pgm_Cmd;
	pxNewListItem->pcHelpString = pgm_CmdDesc;
	pxNewListItem->handler = handler;
	pxNewListItem->what = what;
	xCommandLineInputListItem *entry = interpreter->list;
	//find last element
	while (entry->pxNext!=NULL) {
		entry = entry->pxNext;
	}
	// Now store pointer to this new element to the last element
	entry->pxNext = pxNewListItem;
}
/*-----------------------------------------------------------*/

void CommandLineInterpreter_process(CommandLineInterpreter *interpreter, char *pcCommandInput, Usart *usart) {
	// Search for the command string in the list of registered commands starting with the second entry (first is emtpy)
	for(xCommandLineInputListItem *entry = interpreter->list->pxNext; entry != NULL; entry = entry->pxNext ) {
		if( strcmp_P( ( const char * ) pcCommandInput, ( const char * ) entry->pcCommand ) == 0 ) {
			/* The command has been found, the loop can exit so the command
			can be executed. */
			Handler_sendMessageWithPtr(entry->handler, entry->what, 0, 0, usart);
			return;
		}
	}
	// No matches were found, maybe it was a help command?
	if (strcmp_P( ( const char * ) pcCommandInput, ( const char * ) Strings_HelpCmd ) == 0) {
		Usart_putPgmString(usart, Strings_HelpCmdDesc, 10);
		// Search for the command string in the list of registered commands starting with the second entry (first is emtpy)
		for(xCommandLineInputListItem *entry = interpreter->list->pxNext; entry != NULL; entry = entry->pxNext ) {
			Usart_putPgmString(usart, entry->pcCommand, 10);
			Usart_putPgmString(usart, Strings_colon, 10);
			Usart_putPgmString(usart, Strings_space, 10);
			Usart_putPgmString(usart, entry->pcHelpString, 10);
		}
	} else {
		Usart_putPgmString(usart, Strings_InterpretorError, 10);
	}
}
