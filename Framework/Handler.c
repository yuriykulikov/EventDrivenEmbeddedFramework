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

Message * Handler_obtain(Handler *handler, char what) {
    Message* msg = MsgQueue_obtain(handler->messageQueue);
    msg->handler = handler;
    msg->what = what;
    return msg;
}

void Handler_sendMessage(Handler *handler, Message * message) {
    Handler_sendMessageDelayed(handler, message, 0);
}
void Handler_sendMessageDelayed(Handler *handler, Message *message, unsigned delay) {
    message->due = tick + delay;
    MsgQueue_send(handler->messageQueue, message);
}

/**
 * Post empty message
 * @param handler
 * @param what
 */
void Handler_sendEmptyMessage(Handler *handler, char what) {
    Message* msg = Handler_obtain(handler, what);
    Handler_sendMessage(handler, msg);
}
