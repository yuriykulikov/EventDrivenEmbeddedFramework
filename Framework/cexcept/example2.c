/*===
 * This example was redone by Yuriy Kulikov, to make it little bit more clear.
 *
cexcept: example2.c 2.0.0 (2001-Mar-21-Wed)
Adam M. Costello <amc@cs.berkeley.edu>

An example application that demonstrates how to use version 2.0.* of the
cexcept.h interface to provide polymorphic exceptions, while avoiding
the use of global variables.It also illustrates the use of dynamically
nested Try blocks.

See README for copyright information.

See example.c for a simpler example.

===*/

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#include "exceptions.h"
#include "usart_driver_RTOS.h"
/*
 * This function may throw an exception, that is why it should be called within a try block,
 * passing exception context to it as an argument.
 */
void functionThatMayThrowAnException(USART_buffer_struct_t * usartBuffer, struct exception_context *the_exception_context)
{
	//this is a local variable, memory is allocated for it on the stack
	struct exception e;

	static int count = 0;
	USART_Buffer_PutString(usartBuffer,"Enter functionThatMayThrowAnException",200);
	++count;

	if (count == 2) {
		e.type = null;
		e.msg = "demo null message";
		Throw e;
	}
	else if (count == 3) {
		e.type = outOfHeap;
		e.msg = "demo outOfHeap message";
		Throw e;
	}
	else if (count == 4) {
		e.type = error;
		e.msg = "demo error message";
		Throw e;
	}
	USART_Buffer_PutString(usartBuffer,"Return from functionThatMayThrowAnException",200);
}

/* Bar may throw an exception, that is why it should be called within a try block,
 * passing exception context to it as an argument.
 */
void bar(USART_buffer_struct_t * usartBuffer, struct exception_context *the_exception_context)
{
	USART_Buffer_PutString(usartBuffer, "enter bar\n",200);
	struct exception e;
	Try {
		functionThatMayThrowAnException(usartBuffer, the_exception_context);
	} Catch (e) {
		switch (e.type) {
			case warning:
				//Ignore it
				break;
			case null:
				USART_Buffer_PutString(usartBuffer,"Caught null exception in bar:",200);
				USART_Buffer_PutString(usartBuffer,e.msg,200);
				break;
			//anything else - rethrow the exception
			default: Throw e;
		}
	}
	USART_Buffer_PutString(usartBuffer, "return from bar\n",200);
}

/* This is the main function, which could be called within thread's loop.
 * Thread loop should not be inside the Try block!
 */
void foo(USART_buffer_struct_t * usartBuffer)
{
	//Declare and initialize exception context
	struct exception_context *the_exception_context;
	init_exception_context(the_exception_context);
	//Declare exception itself
	struct exception e;

		Try {
			bar(usartBuffer, the_exception_context);/* no exceptions */
			bar(usartBuffer, the_exception_context);/* exception will be caught by bar(), looks okay to us */
			bar(usartBuffer, the_exception_context);/* bar() will rethrow the exception */
			USART_Buffer_PutString(usartBuffer,"we won't get here\n",200);
		}
		Catch (e) {
			switch (e.type) {
				case warning:
					USART_Buffer_PutString(usartBuffer,"Caught warning exception in foo:",200);
					USART_Buffer_PutString(usartBuffer,e.msg,200);
					break;
				case outOfHeap:
					USART_Buffer_PutString(usartBuffer,"Caught outOfHeap exception in foo:",200);
					USART_Buffer_PutString(usartBuffer,e.msg,200);
					break;
				case error:
					USART_Buffer_PutString(usartBuffer,"Caught something bad in foo:",200);
					USART_Buffer_PutString(usartBuffer,e.msg,200);
				default:
					USART_Buffer_PutString(usartBuffer,"Caught undefined exception in foo:",200);
			}
		}
}
