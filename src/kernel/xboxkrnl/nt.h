#ifndef XBXKRNL_NT_H
#define XBXKRNL_NT_H

#include "kernel/kernel.h"

using XNTSTATUS = int32_t;

#define XSTATUS_SUCCESS   ((XNTSTATUS)0x00000000L)
#define XSTATUS_NO_MEMORY ((XNTSTATUS)0xC0000017L)

XBXKRNL XNTSTATUS
NtAllocateVirtualMemory(ptr32<uint32_t> lpBaseAddress,
                        ptr32<uint32_t> lpRegionSize,
                        uint32_t dwAllocationType,
                        uint32_t dwProtect,
                        uint32_t dwFlags);

#endif // ifndef XBXKRNL_NT_H
