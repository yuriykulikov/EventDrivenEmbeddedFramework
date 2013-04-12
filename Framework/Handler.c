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
#include "Handler.h"

/**
 * Post empty message
 * @param handler
 * @param what
 */
void Handler_sendEmptyMessage(Handler *handler, char what) {
    Message msg = MsgQueue_obtain(handler->messageQueue);
    msg.handler = handler;
    msg.what = what;
    MsgQueue_send(handler->messageQueue, &msg);
}
/**
 * Post message with arguments
 * @param handler
 * @param what
 * @param arg1
 * @param arg2
 */
void Handler_sendMessage(Handler *handler, char what, char arg1, char arg2) {
    Message msg = MsgQueue_obtain(handler->messageQueue);
    msg.handler = handler;
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    MsgQueue_send(handler->messageQueue, &msg);
}
/**
 * Post message with arguments and a pointer to allocated data
 * @param handler
 * @param what
 * @param arg1
 * @param arg2
 * @param ptr
 */
void Handler_sendMessageWithPtr(Handler *handler, char what, char arg1, char arg2, void *ptr) {
    Message msg = MsgQueue_obtain(handler->messageQueue);
    msg.handler = handler;
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    msg.ptr = ptr;
    MsgQueue_send(handler->messageQueue, &msg);
}

/**
 * Creates a handler, which has to be bind to the looper task
 * @param looper
 * @param handleMessage
 * @param context
 */
void Handler_init(Handler *handler, MsgQueue *msgQueue, HANDLE_MESSAGE_CALLBACK handleMessage, void *context) {
    handler->messageQueue = msgQueue;
    handler->handleMessage = handleMessage;
    handler->context = context;
}
