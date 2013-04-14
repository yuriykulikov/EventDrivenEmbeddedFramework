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
#ifndef MESSAGE_H_
#define MESSAGE_H_

typedef struct MESSAGE Message;
typedef struct HANDLER Handler;
typedef struct MSG_QUEUE MsgQueue;

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
    /** Timestamp when the message is due to be handled.*/
    unsigned due;
    /** Next message in the pool or in the queue*/
    Message *next;
};

#endif /* MESSAGE_H_ */
