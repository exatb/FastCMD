# FastCMD
Fast command finding in a text flow for an STM32 microcontroller.

## Includes

`#include "stm32l1xx.h"`: Includes the specific header file for the STM32L1 series microcontrollers. You can change it.

## Definitions

`__ptr uint8_t`: Defines a pointer type and the maximum data size.

`STATIC_BUFFER_SIZE 256`: Defines the static buffer size to 256 bytes for uint8_t pointer type.

## Callback Type

`typedef void(*CommandProcessorCallback)(char*,int)`: Defines a callback function type CommandProcessorCallback that takes a char* argument string and an int argument number.

## Function Declarations

### CharProcess

`CommandProcessorCallback CharProcess(__ptr pcmdnode, char ch)`

Finds commands in the text flow.

**Parameters:**
  
* pcmdnode: Pointer to the command node.
* ch: Character to find.

**Returns:** 0 if the command can't be found or the address of the callback function.

### InitCmdEngine

`void InitCmdEngine(void)`

Clears all commands.

### AddCmd

`char AddCmd(__ptr pcmdset, const char * cmd, CommandProcessorCallback callback)`

Adds a command.

**Parameters:**

* pcmdset: Pointer to the main command object. If *pcmdset == 0, returns the set of commands place.
* cmd: Command string.
* callback: Callback function when the command is found by CharProcess().

**Returns**: 1 if the command is added, 0 if there's not enough memory.

## Example Usage

The provided example demonstrates how to use the command processing functions to find and execute commands from a text flow.

1. Define Callbacks: Define several callback functions for different commands.
2. Initialize Variables: Initialize command sets and find points.
3. Initialize Engine: Call InitCmdEngine() to clear all commands.
4. Add Commands: Use AddCmd() to add various commands to different command sets.
5. Find and Execute Commands: Process a text string to find and execute commands by calling CharProcess() and invoking the callbacks when commands are found.

```
#include "command_processor.h"

char cmdnum=0;
void cmd11_callback(char* arg, int num) { cmdnum = 1; }
void cmd12_callback(char* arg, int num) { cmdnum = 2; }
void cmd13_callback(char* arg, int num) { cmdnum = 3; }
void cmd21_callback(char* arg, int num) { cmdnum = 4; }
void cmd22_callback(char* arg, int num) { cmdnum = 5; }
void cmd23_callback(char* arg, int num) { cmdnum = 6; }

__ptr cmd_set1=0; // command set 1
__ptr cmd_fnd1=0; // for finding in set 1
__ptr cmd_set2=0; // command set 2
__ptr cmd_fnd2=0; // for finding in set 2
CommandProcessorCallback cmd = 0;

InitCmdEngine();
cmd_set1 = 0;
cmd_set2 = 0;
char res = 1;
res &= AddCmd(&cmd_set1,"command1_1",cmd11_callback);
res &= AddCmd(&cmd_set1,"command1_2",cmd12_callback);
res &= AddCmd(&cmd_set1,"command1_3",cmd13_callback);
res &= AddCmd(&cmd_set2,"command2_1",cmd21_callback);
res &= AddCmd(&cmd_set2,"command2_2",cmd22_callback);
res &= AddCmd(&cmd_set2,"command2_3",cmd23_callback);
if (!res) while (1) {}; // out of memory, see STATIC_BUFFER_SIZE and __ptr define
cmd_fnd1 = cmd_set1; // init find point 1
cmd_fnd2 = cmd_set2; // init find point 2

char args[10];
char ai =0;
const char* text1 = "command1_1,command1_2,command1_3";
char* str = (char*)text1;
while (*str) {
  if ((*str)==',') {
    cmd_fnd1 = cmd_set1;
    ai=0;
  } else {
    args[ai++]=*str;
    cmd = CharProcess(&cmd_fnd1, *str);
    if (cmd) {
      args[ai]=0;
      cmd(args,0);
      ai = 0;
    }
  }
  str++;
}

const char* text2 = "command2_1,command2_2,command2_3";
str = (char*)text2;
while (*str) {
  if ((*str)==',') {
    cmd_fnd2 = cmd_set2;
    ai=0;
  } else {
    args[ai++]=*str;
    cmd = CharProcess(&cmd_fnd2, *str);
    if (cmd) {
      args[ai]=0;
      cmd(args,0);
      ai = 0;
    }
  }
  str++;
}
```

