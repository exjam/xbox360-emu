#ifndef XBOXKRNL_TLS_H
#define XBOXKRNL_TLS_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

extern KSpinLock
KiTlsSlotBitmapLock;

uint64_t
KeTlsAlloc();

uint64_t
KeTlsFree(uint32_t index);

ptr32_t<void>
KeTlsGetValue(uint32_t index);

uint64_t
KeTlsSetValue(uint32_t index, ptr32_t<void> value);

#endif // XBXKRNL_TLS_H
