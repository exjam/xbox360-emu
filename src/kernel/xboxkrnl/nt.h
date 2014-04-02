#ifndef NT_H
#define NT_H

#include "kernel/kernel.h"

using XNTSTATUS = XINT;

#define XSTATUS_SUCCESS   ((XNTSTATUS)0x00000000L)
#define XSTATUS_NO_MEMORY ((XNTSTATUS)0xC0000017L)

XBXKRNL XNTSTATUS
NtAllocateVirtualMemory(XLPDWORD BaseAddress,
                        XLPDWORD RegionSize,
                        XDWORD AllocationType,
                        XDWORD Protect,
                        XDWORD Flags);

#endif
