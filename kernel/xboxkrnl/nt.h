#ifndef NT_H
#define NT_H

#include "kernel.h"
#include <Windows.h>

XBXKRNL NTSTATUS
NtAllocateVirtualMemory(PVOID *BaseAddress, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect, ULONG Flags);

#endif
