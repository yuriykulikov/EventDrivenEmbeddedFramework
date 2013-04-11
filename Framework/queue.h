/*
 * queue.h
 *
 *  Created on: 07.04.2013
 *      Author: siralexeyduke
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "handler.h"
#include "looper.h"
#include "hal.h"

#define NULL_MSG 0

void QueueMsgSend( Queue* msgQueue, Message* msg );
Message* QueueMsgGet( Queue* msgQueue );

#endif /* QUEUE_H_ */
