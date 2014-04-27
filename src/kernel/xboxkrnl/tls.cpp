#include "kernel/xboxkrnl/tls.h"

#include <Windows.h>

KSpinLock KiTlsSlotBitmapLock;

uint64_t
KeTlsAlloc()
{
   return TlsAlloc();
}

uint64_t
KeTlsFree(uint32_t index)
{
   return TlsFree(index);
}

ptr32_t<void>
KeTlsGetValue(uint32_t index)
{
   return TlsGetValue(index);
}

uint64_t
KeTlsSetValue(uint32_t index, ptr32_t<void> value)
{
   return TlsSetValue(index, value);
}
