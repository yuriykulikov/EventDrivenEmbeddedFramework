/**
 * strings.h
 *
 * Contains strings, which have to be stored in the program space
 * (flash memory), in order to save precious RAM. Strings have to be
 * read using special functions found in <avr/pgmspace.h>
 *
 * See http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
 */

#ifndef STARINGS_H_
#define STARINGS_H_

// Common
extern char Strings_newline[];
extern char Strings_colon[];
extern char Strings_space[];
extern char Strings_comma[];
extern char Strings_hex[];
// Spi task
extern char Strings_SpiMasterExample1[];
extern char Strings_SpiMasterExample2[];
extern char Strings_SpiMasterExample3[];
extern char Strings_SpiSlaveExample1[];

extern char Strings_SpiExampleCmd[];
extern char Strings_SpiExampleCmdDesc[];
extern char Strings_SpiExampleCmdResp[];
// CommandInterpreterTask
extern char Strings_BlinkCmd[];
extern char Strings_BlinkCmdDesc[];
extern char Strings_BlinkResp[];
extern char Strings_BlinkCmdError[];
// CommandInterpreter
extern char Strings_InterpretorError[];
extern char Strings_HelpCmd[];
extern char Strings_HelpCmdDesc[];

#endif /* STARINGS_H_ */
