/*
 * Copyright (C) 2013 Yuriy Kulikov yuriy.kulikov.87@gmail.com
 * Copyright (C) 2013 Alexey Serdyuk
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "MsgQueue.h"
#include "../hal/hal.h"
/*
 * @startuml
 * Participant Client
 * Participant Handler
 * Participant Queue
 * Participant main
 *
 * note over Queue: pool    head->[m1]->[m2]->[m3]->0\nqueue head->[m4]->[m5]->[m6]->0
 * Client -> Handler : obtainMessage
 * Handler -> Queue : obtainMessage
 * Queue -> Queue : etract head from the pool, move the head
 * note over Queue: pool    head->[m2]->[m3]->0\nqueue head->[m4]->[m5]->[m6]->0
 * Queue-->Handler : [m1]
 * Handler --> Client : [m1]
 * Client -> Client : assign what, args
 * Client -> Handler : sendMessage ( [m1] )
 * Handler -> Handler : set due time to tick (to be handled immedeately)
 * Handler -> Queue : send( [m1] )
 * Queue -> Queue : insert the message, update head if required
 * note over Queue: [m6] is due after the [m1], but [m5] and [m4] are due before
 * note over Queue: pool    head->[m2]->[m3]->0\nqueue head->[m4]->[m5]-><b>[m1]</b>->[m6]->0
 * ...some time passed - interrupt ...
 * main -> Queue : processNextMessage
 * Queue -> Queue : remove head from the queue
 * note over Queue: pool    head->[m2]->[m3]->0\nqueue <b>head->[m5]</b>->[m1]->[m6]->0
 * Queue->Handler : handleMessage( [m4] )
 * Queue -> Queue : recycle the message
 * note over Queue: pool    <b>head->[m4]</b>->[m2]->[m3]->0\nqueue head->[m5]->[m1]->[m6]->0
 * Queue --> main
 * @enduml
 */

unsigned tick;

void MsgQueue_init(MsgQueue* msgQueue, unsigned poolSize) {
    msgQueue->poolHead = MsgArray;
    msgQueue->queueHead = 0;
    MsgArray[0].next=&MsgArray[1];
    int i;
    for( i = 0; i<QUEUE_MAX_LEN-1; i++)
    	MsgArray[i].next=&MsgArray[i+1];
}

Message* MsgQueue_obtain(MsgQueue* msgQueue) {
    //if the pool is emtpy, we will get a nullpointer, everything will fail, but this should never happen
    Message *msg = msgQueue->poolHead;
    msgQueue->poolHead = msgQueue->poolHead->next;
    return msg;
}

void MsgQueue_send(MsgQueue* msgQueue, Message* msg) {
    DISABLE_INTERRUPTS;
    if (msgQueue->queueHead == 0) {
        //if queue is empty, simply put the message into the queue
        msgQueue->queueHead = msg;
        //and there is no next message
        msg->next = 0;
    } else {
        //traverse the queue looking for the insertion point
        //here is a queue, due times in brackets.
        //we have to put [300] into the [10][11][100][400]
        //[10][11] previous = [100] next = [400], since next-> is more than 300
    	Message *previous;
        for (previous = msgQueue->queueHead; previous != 0; previous = previous->next) {
            Message *next = previous->next;
            if (next == 0) {
                //reached the end of queue, but our message is due later than the last message in the queue
                previous->next = msg;
                msg->next = 0;
                break;
            } else if (next->due > msg->due) {
                //next message is due later than our message. We have to insert out message between the next and the previous
                //here we use > and not >=, because of FIFO
                previous->next = msg;
                msg->next = next;
                break;
            }
        }
    }
    ENABLE_INTERRUPTS;
}

void MsgQueue_processNextMessage(MsgQueue* msgQueue) {
    Message *msg = msgQueue->queueHead;
    //if there is a message left in the queue
    if (msg != 0) {
        //if this is about time to handle
        if (msg->due <= tick) {
            //move the head
            msgQueue->queueHead = msg->next;
            Handler* handler = msg->handler;
            //handle the message
            handler->handleMessage(*msg, handler->context, handler);
            //recycle the message so that it will be placed back into the pool
            MsgQueue_recycle(msgQueue, msg);
        }
    }
}

void MsgQueue_recycle(MsgQueue* msgQueue, Message* msg) {
    //we have to put this mesasge back into the pool
    //now this message will point to the old head
    msg->next = msgQueue->poolHead;
    //and the new head will point to this message
    msgQueue->poolHead = msg;
}
