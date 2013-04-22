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
#ifndef HANDLER_H_
#define HANDLER_H_

#include "portmacro.h"

typedef struct HANDLER Handler;
typedef struct MESSAGE Message;
typedef struct MSG_QUEUE MsgQueue;

#define NULL_CONTEXT 0

/**
 * The prototype to which handleMessage functions used to process messages must comply.
 * @param msg - message to handle
 * @param *context - context of current Handler. Store variables there
 * @param *handler - pointer to the handler, to modify or look into the queue
 */
typedef void (*HANDLE_MESSAGE_CALLBACK)(Message msg, void *context, Handler *handler);

/** Struct represents handler */
struct HANDLER {
    /** Pointer to queue on which handler posts messages */
    MsgQueue* messageQueue;
    /** Function which handles messages*/
    HANDLE_MESSAGE_CALLBACK handleMessage;
    /** Execution context of current handler, handleMessage should cast it to something */
    void *context;
};

/**
 * This structure represents a message
 */
struct MESSAGE {
    /** Handler responsible for handling this message */
    Handler *handler;
    /** What message is about */
    portBASE_TYPE what;
    /** First argument */
    portBASE_TYPE arg1;
    /** First argument */
    portBASE_TYPE arg2;
    /** Pointer to the allocated memory. Handler should cast to the proper type,
     * according to the message.what */
    void *ptr;
    /** Timestamp when the message is due to be handled.*/
    uint32_t due;
    /** Next message in the pool or in the queue*/
    Message *next;
};

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
void MsgQueue_init(MsgQueue* msgQueue, Message* pool, portBASE_TYPE poolSize);

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

void Handler_init(Handler *handler, MsgQueue *msgQueue, HANDLE_MESSAGE_CALLBACK handleMessage, void *context);
Message * Handler_obtain(Handler *handler, portBASE_TYPE what);
void Handler_sendMessage(Handler *handler, Message * message);
void Handler_sendMessageDelayed(Handler *handler, Message * message, portLONG delay);
void Handler_sendEmptyMessage(Handler *handler, portBASE_TYPE what);

#endif /* HANDLER_H_ */
