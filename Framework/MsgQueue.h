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

#ifndef QUEUE_H_
#define QUEUE_H_

#include "Handler.h"
#include "Message.h"

#define QUEUE_MAX_LEN 20

typedef struct MESSAGE Message;
typedef struct HANDLER Handler;
typedef struct MSG_QUEUE MsgQueue;

/** Pool of messages */
static Message MsgArray[QUEUE_MAX_LEN];

//static unsigned tick;
extern unsigned tick;

struct MSG_QUEUE {
    Message *poolHead;
    Message *queueHead;
};

/**
 *Initialize the queue. poolHead must point to the head of the area of allocated memory.
 * @param msgQueue
 * @param poolHead
 * @param poolSize
 */
void MsgQueue_init(MsgQueue* msgQueue, unsigned poolSize);

/**
 * Put the message into the queue.
 * @param msgQueue
 * @param msg
 */
void MsgQueue_send(MsgQueue* msgQueue, Message* msg);

/**
 * Obtain a message from a message pool. Send the message once all the parameters are set.
 * If message will not be used, call MsgQueue_recycle.
 * @param msgQueue
 * @return
 */
Message* MsgQueue_obtain(MsgQueue* msgQueue);

/**
 * Put the message back into the pool.
 * @param msgQueue
 * @param msg
 */
void MsgQueue_recycle(MsgQueue* msgQueue, Message* msg);

/**
 * Process the next message and recycle it.
 * @param msgQueue
 */
void MsgQueue_processNextMessage(MsgQueue* msgQueue);
#endif /* QUEUE_H_ */
