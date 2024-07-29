/** 
\file

\brief Fast find commands in the text flow 

*/
#pragma once

#include "stm32l1xx.h"

/*max data size = 256*/
#define __ptr uint8_t /**<Select type of pointers and maximum data size*/
#define STATIC_BUFFER_SIZE 256 /**<Select data size*/

/**
\brief Callback function type
\param [in] char* Argument string
\param [in] int Argument number
*/
typedef void(*CommandProcessorCallback)(char*,int); /**<Callback function type*/

/**
\brief Commands find process
\param [in,out] pcmdnode Find node of commands
\param [in] ch Char for finding
\return 0 can't find command or addres of callback function
*/
CommandProcessorCallback CharProcess(__ptr* pcmdnode, char ch); 

void InitCmdEngine(void); /*clear all commands*/

/**
\brief Command adding
\param [in,out] pcmdset Main cmd object, in first run (*pcmdset==0) returns set of commands place
\param [in] cmd Command string
\param [in] callback Callback then cmd is found by CharProcess()
\return 1 - cmd added, 0 - not enouth memory
*/
char AddCmd(__ptr* pcmdset, const char * cmd, CommandProcessorCallback callback);

/**
\example

Example:

...

char cmdnum=0;
void cmd11_callback(char* arg, int num) { cmdnum = 1; }
void cmd12_callback(char* arg, int num) { cmdnum = 2; }
void cmd13_callback(char* arg, int num) { cmdnum = 3; }
void cmd21_callback(char* arg, int num) { cmdnum = 4; }
void cmd22_callback(char* arg, int num) { cmdnum = 5; }
void cmd23_callback(char* arg, int num) { cmdnum = 6; }

...

__ptr cmd_set1=0; //commnad set 1
__ptr cmd_fnd1=0; //for find in set 1
__ptr cmd_set2=0; //commnad set 2
__ptr cmd_fnd2=0; //for find in set 2
CommandProcessorCallback cmd = 0;

...

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
if (!res) while (1) {}; //out of memory, see STATIC_BUFFER_SIZE and __ptr define
cmd_fnd1 = cmd_set1; //init find point 1
cmd_fnd2 = cmd_set2; //init find point 2

...

char args[10];
char ai =0;
const char* text1 = "command1_1,command1_2,command1_3";
char* str = (char*)text1;
while (*str)
{
  if ((*str)==',') 
  {
    cmd_fnd1 = cmd_set1;
    ai=0;
  }
  else
  {
    args[ai++]=*str;    
    cmd = CharProcess(&cmd_fnd1, *str);
    if (cmd) 
    {
      args[ai]=0;
      cmd(args,0);
      ai = 0;
    }
  }
  str++;
}

...

const char* text2 = "command2_1,command2_2,command2_3";
str = (char*)text2;
while (*str)
{
  if ((*str)==',') 
  {
    cmd_fnd2 = cmd_set2;
    ai=0;
  }
  else
  {
    args[ai++]=*str;    
    cmd = CharProcess(&cmd_fnd2, *str);
    if (cmd) 
    {
      args[ai]=0;
      cmd(args,0);
      ai = 0;      
    }
  }
  str++;
}  

...

*/