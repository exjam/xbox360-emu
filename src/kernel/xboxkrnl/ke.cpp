#include "ke.h"

#include <Windows.h>

uint32_t KeDebugMonitorData = 0;

enum class ProcessType
{
   Idle = 0,
   User = 1,
   System = 2
};

XBXKRNL uint8_t
KeGetCurrentProcessType()
{
   return static_cast<uint8_t>(ProcessType::User);
}

XBXKRNL uint64_t
KeTlsAlloc()
{
   return 0;
}

XBXKRNL uint64_t
KeTlsFree(uint32_t dwTlsIndex)
{
   return 0;
}

XBXKRNL uint32_t
KeTlsGetValue(uint32_t dwTlsIndex)
{
   return 0;
}

XBXKRNL uint64_t
KeTlsSetValue(uint32_t dwTlsIndex,
              uint32_t lpTlsValue)
{
   return 0;
}

XBXKRNL XNTSTATUS
KeWaitForSingleObject(ptr32<void> Object,
                      uint32_t WaitReason,
                      uint32_t WaitMode,
                      uint32_t Alertable,
                      ptr32<uint64_t> Timeout)
{
   return 0;
}
