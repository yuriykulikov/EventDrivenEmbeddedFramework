/*
 * Copyright (C) 2012 Yuriy Kulikov
 *      Universitaet Erlangen-Nuernberg
 *      LS Informationstechnik (Kommunikationselektronik)
 *      Support email: Yuriy.Kulikov.87@googlemail.com
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

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "avr_compiler.h"

#define BUTTON_CHECK_PERIOD 100

#if (CLICK_DURATION>0xFF)
	#error "hey thats too much!"
#endif
#if (LONG_CLICK_DURATION>0xFF)
	#error "hey thats too much!"
#endif

typedef void (*ON_CLICK_CALLBACK)();
typedef void (*ON_LONG_CLICK_CALLBACK)();

typedef struct Button_struct_defenition
{
	int16_t Counter;
	uint8_t * Port;
	uint8_t Mask;
	ON_CLICK_CALLBACK onClick;
	ON_LONG_CLICK_CALLBACK onLongClick;
} Button_struct_t;

void Button_checkButton(Button_struct_t * Button);
void Button_init(Button_struct_t * Button, uint8_t *port, uint8_t *pullupPort, uint8_t mask, ON_CLICK_CALLBACK onClick,
	ON_LONG_CLICK_CALLBACK onLongClick);

#endif /* BUTTONS_H_ */
