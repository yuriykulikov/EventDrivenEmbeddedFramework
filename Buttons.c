/*
 * Copyright (C) 2013 Yuriy Kulikov
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

#include "Buttons.h"

/**
 * @param pointer to button structure which should be checked
 */
void Button_checkButton(Button_struct_t * Button) {
	/* button is released - we should do nothing if it happened after longclick. And if it was a click
	 * we should report it.	Think of button as a one-element queue. Counter variable represents how long
	 *  the button is pressed - it is only cleared when the button is released*/
	if (((*(Button->Port)) & Button->Mask) == 0) {  //if button is pressed
	//if the button is not active at the moment increment counter by one, which is 10 ms
		if (Button->Counter == LONG_CLICK_DURATION) {//long click state is achieved
			Button->onLongClick();
			Button->Counter = -1;
		}
		if (Button->Counter != -1) {
			Button->Counter++;
		}
	} else {
		//First take a look what do we have in counter
		if (Button->Counter >= CLICK_DURATION) {
			Button->onClick();
		}
		Button->Counter = 0;		//if button is released - clear counter
	}
}

void Button_init(Button_struct_t * Button, uint8_t *inputPort, uint8_t mask, ON_CLICK_CALLBACK onClick,
		ON_LONG_CLICK_CALLBACK onLongClick) {
	Button->onClick = onClick;
	Button->onLongClick = onLongClick;
	Button->Port = inputPort;
	Button->Mask = mask;
	Button->Counter = 0;
}
