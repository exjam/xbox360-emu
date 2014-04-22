#ifndef XBXKRNL_THREAD_H
#define XBXKRNL_THREAD_H

#include "kernel/kernel.h"

XBXKRNL uint32_t
ExCreateThread(ptr32<uint32_t> pHandle,
               uint32_t dwStackSize,
               ptr32<uint32_t> lpThreadId,
               ptr32<void*> apiThreadStartup,
               ptr32<void*> lpStartAddress,
               ptr32<void*> lpParameter,
               uint32_t dwCreationFlagsMod);

#endif // ifndef XBXKRNL_THREAD_H
