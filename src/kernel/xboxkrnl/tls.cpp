#include "kernel/xboxkrnl/tls.h"

//TODO: Un-windows-ify
#include <Windows.h>

KSpinLock KiTlsSlotBitmapLock;

uint64_t
KeTlsAlloc()
{
   return TlsAlloc();
}

KBOOL
KeTlsFree(uint32_t index)
{
   return TlsFree(index);
}

ptr32_t<void>
KeTlsGetValue(uint32_t index)
{
   return TlsGetValue(index);
}

KBOOL
KeTlsSetValue(uint32_t index, ptr32_t<void> value)
{
   return TlsSetValue(index, value);
}
