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
/* Scheduler include files. */

typedef struct MESSAGE Message;
typedef struct HANDLER Handler;
typedef struct QUEUE Queue;

#include "queue.h"
#include "looper.h"

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
    Queue* messageQueue;
    //Queue messageQueue;
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
    char what;
    /** First argument */
    char arg1;
    /** First argument */
    char arg2;
    /** Pointer to the allocated memory. Handler should cast to the proper type,
     * according to the message.what */
    void *ptr;
};

///**

/**
 * This structure represents a queue
 */
#define QUEUE_MAX_LEN 20
struct QUEUE {
    /** Array of messages */
    Message MsgArray[QUEUE_MAX_LEN];
    /** Queue top */
    char qTop;
    /** Queue tail */
    char qTail;
};

/* Prototyping of functions. Documentation is found in source file. */

//Handler * Handler_create(Looper *looper, HANDLE_MESSAGE_CALLBACK handleMessage, void *context);
void Handler_sendEmptyMessage(Handler *handler, char what);
void Handler_sendMessage(Handler *handler, char what, char arg1, char arg2);
void Handler_sendMessageWithPtr(Handler *handler, char what, char arg1, char arg2, void *ptr);

#endif /* HANDLER_H_ */
