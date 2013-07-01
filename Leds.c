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
#include "Leds.h"

/**
 * Led group is a group of leds :-)
 * An example could be and RGB led - which is a group of 3 leds. Or a led bar, consisting of 8 leds.
 * The idea is that a after initialization each led of the group is addressable with a bitmask or its numbers.
 * This allows to map real leds to different ports and pins, meanwhile the algorithm (sliding light or progress
 * bar) remains unchanged and easy.
 */
void Led_init(Led *led, uint8_t *port, uint8_t bitmask, uint8_t isActiveLow) {
    led->bitmask = bitmask;
    led->port = port;
    led->isActiveLow = isActiveLow;
}

void Led_set(Led *led) {
    if (led->isActiveLow) {
        *(led->port) &= ~led->bitmask;
    } else {
        *(led->port) |= led->bitmask;
    }
}

void Led_clear(Led *led) {
    if (!led->isActiveLow) {
        *(led->port) &= ~led->bitmask;
    } else {
        *(led->port) |= led->bitmask;
    }
}

void LedGroup_init(LedGroup *ledGroup, Led *array) {
    ledGroup->amountOfLeds = 0;
    ledGroup->leds = array;
}
/*
 * Adds new led to group. Note that there is important to add
 * leds in the order, because they will be addressed by index.
 * Only 8 leds are supported.
 * Special case for RGB - add Red, then Green, then Blue.
 * Special case for RG - add Red, then Green
 */
void LedGroup_add(LedGroup *ledGroup, Led *led) {
    if (ledGroup->amountOfLeds < 8) {
        ledGroup->leds[ledGroup->amountOfLeds] = led;
        //we have added one more led to the group
        ledGroup->amountOfLeds++;
    }
}

/*
 * Sets leds shining according to the bitmask. If you have 8 leds and want
 * leds 1,5 and 8 shining, pass bitmask '10010001'=0x91.
 * It is possible to use Color_enum instead of the bitmask for RGB leds -
 * in this case R G and B lesa should added in this order - R G B
 */
void LedGroup_set(LedGroup *ledGroup, uint8_t bitmask) {
    for (uint8_t i = 0; i < ledGroup->amountOfLeds; i++) {
        /* bitmask>>i shifts bitmask to the left, effectively placing bit
         * number i to the 0x01 position. Than clear all other bits and
         * see if this bit was 1
         */
        if (((bitmask >> i) & 0x01)) {
            Led_set(ledGroup->leds[i]);
        } else {
            Led_clear(ledGroup->leds[i]);
        }
    }
}
