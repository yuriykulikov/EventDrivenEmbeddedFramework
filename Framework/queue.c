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
#include "queue.h"

void QueueMsgSend(Queue* msgQueue, Message* msg) {
    DISABLE_INTERRUPTS;
    if (msgQueue->qTop < QUEUE_MAX_LEN - 1)
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

Message* QueueMsgGet(Queue* msgQueue) {
    Message* this;
    this = &msgQueue->MsgArray[msgQueue->qTop];
    if (msgQueue->qTop != msgQueue->qTail) {
        msgQueue->MsgArray[msgQueue->qTail].what = 0;
        if (msgQueue->qTail < QUEUE_MAX_LEN - 1)
            msgQueue->qTail++;
        else
            msgQueue->qTail = 0;
    } else
        msgQueue->MsgArray[msgQueue->qTail].what = NULL_MSG;

    return this;
}

