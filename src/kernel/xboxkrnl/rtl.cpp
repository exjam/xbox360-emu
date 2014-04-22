#include "rtl.h"
#include "common/memory.h"

#include <algorithm>

XBXKRNL void
RtlInitializeCriticalSection(ptr32<KCriticalSection> lpCriticalSection)
{
   return RtlInitializeCriticalSectionAndSpinCount(lpCriticalSection, 0);
}

XBXKRNL void
RtlInitializeCriticalSectionAndSpinCount(ptr32<KCriticalSection> lpCriticalSection,
                                         uint32_t dwSpinCount)
{
   lpCriticalSection->LockCount = -1;
   lpCriticalSection->RecursionCount = 0;
   lpCriticalSection->OwningThread = 0;
   lpCriticalSection->Header.Absolute = std::max((dwSpinCount + 0xff) >> 8, 0xffu);
}

XBXKRNL uint64_t
RtlTryEnterCriticalSection(ptr32<KCriticalSection> lpCriticalSection)
{
   return 0;
}

XBXKRNL void
RtlEnterCriticalSection(ptr32<KCriticalSection> lpCriticalSection)
{

}

XBXKRNL void
RtlLeaveCriticalSection(ptr32<KCriticalSection> lpCriticalSection)
{

}
