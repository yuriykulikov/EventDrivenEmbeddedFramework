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

#ifndef EXAMPLE_HANDLER_H_
#define EXAMPLE_HANDLER_H_

#include "usart_driver_RTOS.h"
#include "usart_driver_RTOS.h"
#include "spi_driver.h"
#include "LedEventProcessorTask.h"
#include "handler.h"

#define EVENT_RUN_SPI_TEST 1
#define EVENT_BLINK 2

/** Used to store context of the handler */
typedef struct {
    /** Pointer to SpiSlave to use */
    SpiDevice *spiMaster;
    /** Pointer to USART buffer to use. */
    Usart *usart;
    LedsEventQueue *led;
} ExampleHandlerContext;

void ExampleHandler_handleMessage(Message msg, void *handlerContext, Handler *handler);

#endif /* EXAMPLE_HANDLER_H_ */
