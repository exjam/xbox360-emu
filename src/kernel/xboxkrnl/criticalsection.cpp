#include "kernel/xboxkrnl/criticalsection.h"
#include "common/memory.h"

#include <algorithm>

void
RtlInitializeCriticalSection(ptr32_t<KCriticalSection> cs)
{
   return RtlInitializeCriticalSectionAndSpinCount(cs, 0);
}

void
RtlInitializeCriticalSectionAndSpinCount(ptr32_t<KCriticalSection> cs,
                                         uint32_t spin)
{
   cs->LockCount = -1;
   cs->RecursionCount = 0;
   cs->OwningThread = 0;
   cs->Header.Absolute = std::max((spin + 0xff) >> 8, 0xffu);
}

uint64_t
RtlTryEnterCriticalSection(ptr32_t<KCriticalSection> cs)
{
   return 0;
}

void
RtlEnterCriticalSection(ptr32_t<KCriticalSection> cs)
{

}

void
RtlLeaveCriticalSection(ptr32_t<KCriticalSection> cs)
{

}
