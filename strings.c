/**
 * strings.h
 *
 * Contains strings, which have to be stored in the program space
 * (flash memory), in order to save precious RAM. Strings have to be
 * read using special functions found in <avr/pgmspace.h>
 *
 * See http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
 */
#include <avr/pgmspace.h>

// Common
char Strings_newline[] PROGMEM = "\n";
// Spi task
char Strings_SpiMasterExample1[] PROGMEM = "Master send: 0xC001, received: 0x";
char Strings_SpiMasterExample2[] PROGMEM = "Master send: 0xC0DE, received: 0x";
char Strings_SpiMasterExample3[] PROGMEM = "Master send: 0xD000DE, received: 0x";
char Strings_SpiSlaveExample1[] PROGMEM = "Slave received: 0x";

char Strings_SpiExampleCmd[] PROGMEM = "test spi";
char Strings_SpiExampleCmdDesc[] PROGMEM = "test spi: Start SPI Master Test\n";
char Strings_SpiExampleCmdResp[] PROGMEM = "starting SPI Test...\n";
// CommandInterpreterTask
char Strings_BlinkCmd[] PROGMEM = "blink";
char Strings_BlinkCmdDesc[] PROGMEM = "blink: Blinks the LED\n";
char Strings_BlinkResp[] PROGMEM = "Blinking...\n";
char Strings_BlinkCmdError[] PROGMEM = "blink error: no led assigned\n";
// CommandInterpreter
char Strings_InterpretorError[] PROGMEM = "Command not recognized.  Enter \"help\" to view a list of available commands.\n";
char Strings_HelpCmd[] PROGMEM = "help";
char Strings_HelpCmdDesc[] PROGMEM = "help: Lists all the registered commands\n";
