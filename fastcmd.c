#include "fastcmd.h"

/*max data size = 256*/
#define __ptr uint8_t 

#pragma pack(push, 1)

typedef struct {
  char ch;
  __ptr x; /*next char of command*/
  __ptr y; /*first char list*/
}CmdNode ;

#pragma pack(pop)

#define STATIC_BUFFER_BEGIN 1 /*0 index for error*/

/*static buffer*/
volatile char static_buffer[STATIC_BUFFER_SIZE];

/*  convert (__prt) to (void*)  */
#define SMEM2PTR(ptr) ((void*)(static_buffer+ptr))

/*  convert (__prt) to (uint32_t*)  */
#define SMEM2PUINT32_T(ptr) ((uint32_t*)SMEM2PTR(ptr))

/*  convert (__prt) to (CmdNode*)  */
#define SMEM2PNODE(ptr) ((CmdNode*)SMEM2PTR(ptr))

/*pointer in static buffer and allocated size*/
volatile int StaticBufferSize = STATIC_BUFFER_BEGIN;

CommandProcessorCallback cmd_callback = 0; /*callback of command*/
const char* str; /*command string*/
int chi; /*index in command string*/

/*allocate memory, size in bytes,       */
/*return offset from the static buffer  */
__ptr s_getmem(int size)
{
  __ptr res = 0;
  if ((StaticBufferSize+size)<=STATIC_BUFFER_SIZE)
  {        
    res=StaticBufferSize;
    StaticBufferSize+=size;
  }
  return res;
}

char FindY(__ptr* pnode, char ch)
{
  while (1)   
  {
    if (!pnode) return 0;
    if (!*pnode) return 0;
    CmdNode * pt = SMEM2PTR(*pnode);
    if (pt->ch==ch) return 1; /*if found*/
    if (pt->y)
      *pnode=pt->y;
    else
      return 0; /*but parameter pnode returns end of list (y)*/
  }
}

char AddCh(__ptr* pnode)
{
  if (!str[chi]) 
  {
    __ptr p = s_getmem(sizeof(void*));
    if (!p) return 0;
    *SMEM2PUINT32_T(p)=(uint32_t)cmd_callback; /*set up callback*/
    return 1; /*end of command*/
  }
  if (!pnode) return 0; /*error*/
  if (!*pnode)
  {    
    *pnode = s_getmem(sizeof(CmdNode));
    if (!*pnode) return 0;
    SMEM2PNODE(*pnode)->x=0;
    SMEM2PNODE(*pnode)->y=0;
    SMEM2PNODE(*pnode)->ch=str[chi];
    return AddCh(pnode);
  }else
  {
    __ptr temp = *pnode;
    if (FindY(&temp, str[chi]))
    {
      chi++;      
      return AddCh(&SMEM2PNODE(temp)->x);
    }else
    {
      return AddCh(&SMEM2PNODE(temp)->y);
    }
  }     
}

char AddCmd(__ptr* pcmdset, const char * cmd, CommandProcessorCallback callback)
{
  str = cmd;
  chi=0;
  cmd_callback = callback;  
  return AddCh(pcmdset);
}

void InitCmdEngine(void)
{
  StaticBufferSize = STATIC_BUFFER_BEGIN;
  for (int i=0;i<STATIC_BUFFER_SIZE;i++) static_buffer[i]=0;
}

CommandProcessorCallback CharProcess(__ptr* pcmdnode, char ch)
{
  if (FindY(pcmdnode, ch))
  {
    if (SMEM2PNODE(*pcmdnode)->x) /*if next char exists*/
    {
      *pcmdnode=SMEM2PNODE(*pcmdnode)->x; /*find in next iteration*/
    }else /*end of command (x==0)*/      
      return (CommandProcessorCallback)(*((uint32_t*)(&(SMEM2PNODE(*pcmdnode)->y)+sizeof(__ptr))));
  }
  return 0;
}


