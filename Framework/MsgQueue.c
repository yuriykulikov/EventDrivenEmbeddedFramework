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

void MsgQueue_init(MsgQueue* msgQueue, Message* poolHead, uint16_t poolSize) {
    msgQueue->poolHead = poolHead;
    msgQueue->queueHead = 0;
}

Message* MsgQueue_obtain(MsgQueue* msgQueue) {
    //if the pool is emtpy, we will get a nullpointer, everything will fail, but this should never happen
    Message *msg = msgQueue->poolHead;
    msgQueue->poolHead->next;
    return msg;
}

void MsgQueue_send(MsgQueue* msgQueue, Message* msg) {
    //TODO
    DISABLE_INTERRUPTS;
    //allright this is tricky and not for an old guy like me.
    //Besides my wife makes one hell of a LongIslandIcedTea :-)
    //so I only put todos
    Message *before;
    Message *after;
    //TODO loop through the queue to find an insertion placer using msg->due
    //after this we will have 2 pointers: Message *before and Message *after
    //one of these can be 0
    if (before == 0) {
        msgQueue->queueHead = msg;
    } else {
        before->next = msg;
    }

    if (after == 0) {
        msg->next = 0;
    } else {
        msg->next = after;
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
            handler->handleMessage(msg, handler->context, handler);
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
