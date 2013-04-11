/*
 * queue.c
 *
 *  Created on: 07.04.2013
 *      Author: siralexeyduke
 */
#include "queue.h"

void QueueMsgSend( Queue* msgQueue, Message* msg)
{
    DISABLE_INTERRUPTS;
    if(msgQueue->qTop < QUEUE_MAX_LEN-1)
        msgQueue->qTop++;
    else
        msgQueue->qTop = 0;
    msgQueue->MsgArray[msgQueue->qTop].what = msg->what;
    msgQueue->MsgArray[msgQueue->qTop].handler = msg->handler;
    msgQueue->MsgArray[msgQueue->qTop].ptr = msg->ptr;
    msgQueue->MsgArray[msgQueue->qTop].arg1 = msg->arg1;
    msgQueue->MsgArray[msgQueue->qTop].arg2 = msg->arg2;
    ENABLE_INTERRUPTS;
}

Message* QueueMsgGet( Queue* msgQueue )
{
    Message* this;
    this = &msgQueue->MsgArray[msgQueue->qTop];
    if(msgQueue->qTop != msgQueue->qTail)
    {
        msgQueue->MsgArray[msgQueue->qTail].what = 0;
        if(msgQueue->qTail < QUEUE_MAX_LEN-1)
            msgQueue->qTail++;
        else
            msgQueue->qTail = 0;
    }
    else
        msgQueue->MsgArray[msgQueue->qTail].what = NULL_MSG;

    return this;
}

