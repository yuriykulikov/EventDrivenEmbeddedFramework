-------------IMPORTANT---------------
Do not modify files tasks.c queue.c list.c and others.
The only files that could be modified are: FreeRTOSconfig.h and files in folder RTOS/portable
For ANY XMEGA this  port in current state should work. The only difference between ports
for different xmegas is size of program counter. 16-bit for devices with 128K of program memory
or less. 24-bit for other xmegas. This only matters in pxPortInitialiseStack();.
You will receive error from compiler if amount of memory for your devices is not defined
(you will have to do it yourself).

Do not do any other modifications if you are not absolutely sure.

in RTOS/portable/memMang exclude files heap_2.c and heap_3.c from build.
