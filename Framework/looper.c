/*
 * looper.c
 *
 *  Created on: 07.04.2013
 *      Author: Алексей
 */
#include "looper.h"

void Looper( Queue* messageQueue )
{
    Message* this;
    this = QueueMsgGet( messageQueue );
    Handler* handler = this->handler;
    handler->handleMessage(*this, this->ptr, this->handler );
}
