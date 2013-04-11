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
#include <avr/pgmspace.h>

// Common
char Strings_newline[] PROGMEM = "\n";
char Strings_colon[] PROGMEM = ":";
char Strings_space[] PROGMEM = " ";
char Strings_comma[] PROGMEM = ",";
char Strings_hex[] PROGMEM = "0x";

// Spi task
char Strings_SpiMasterExample1[] PROGMEM = "Master send: 0xC001, received: 0x";
char Strings_SpiMasterExample2[] PROGMEM = "Master send: 0xC0DE, received: 0x";
char Strings_SpiMasterExample3[] PROGMEM = "Master send: 0xD000DE, received: 0x";
char Strings_SpiSlaveExample1[] PROGMEM = "Slave received: 0x";

char Strings_SpiExampleCmd[] PROGMEM = "test spi";
char Strings_SpiExampleCmdDesc[] PROGMEM = "Start SPI Master Test\n";
char Strings_SpiExampleCmdResp[] PROGMEM = "starting SPI Test...\n";
// CommandInterpreterTask
char Strings_BlinkCmd[] PROGMEM = "blink";
char Strings_BlinkCmdDesc[] PROGMEM = "Blinks the LED\n";
char Strings_BlinkResp[] PROGMEM = "Blinking...\n";
char Strings_BlinkCmdError[] PROGMEM = "blink error: no led assigned\n";
// CommandInterpreter
char Strings_InterpretorError[] PROGMEM
= "Command not recognized.  Enter \"help\" to view a list of available commands.\n";
char Strings_HelpCmd[] PROGMEM = "help";
char Strings_HelpCmdDesc[] PROGMEM = "help: Lists all the registered commands\n";
