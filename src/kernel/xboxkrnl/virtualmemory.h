#ifndef XBOXKRNL_VIRTUAL_MEMORY_H
#define XBOXKRNL_VIRTUAL_MEMORY_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

KNTSTATUS
NtAllocateVirtualMemory(ptr32_t<be_uint32_t> lpBaseAddress,
                        ptr32_t<be_uint32_t> lpRegionSize,
                        uint32_t dwAllocationType,
                        uint32_t dwProtect,
                        uint32_t dwFlags);

#endif // ifndef XBOXKRNL_VIRTUAL_MEMORY_H
