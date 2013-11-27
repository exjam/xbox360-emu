#ifndef THREAD_H
#define THREAD_H

#include <kernel.h>

XBXKRNL XDWORD
ExCreateThread(XLPHANDLE pHandle,
               XDWORD dwStackSize,
               XLPDWORD lpThreadId,
               XLPVOID apiThreadStartup,
               XLPVOID lpStartAddress,
               XLPVOID lpParameter,
               XDWORD dwCreationFlagsMod);

#endif
