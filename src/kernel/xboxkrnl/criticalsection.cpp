#include "kernel/xboxkrnl/criticalsection.h"
#include "kernel/xboxkrnl/thread.h"
#include "kernel/xboxkrnl/object.h"
#include "common/memory.h"

#include <algorithm>

static_assert(sizeof(std::atomic_uint32_t) == 4, "Atomic 4");

KSTATUS
RtlInitializeCriticalSection(ptr32_t<KCriticalSection> cs)
{
   return RtlInitializeCriticalSectionAndSpinCount(cs, 0);
}

KSTATUS
RtlInitializeCriticalSectionAndSpinCount(ptr32_t<KCriticalSection> cs,
                                         uint32_t spin)
{
   cs->Header.Type = EventSynchronizationObject;
   cs->Header.SignalState = 0;
   cs->LockCount = -1;
   cs->RecursionCount = 0;
   cs->OwningThread = nullptr;
   cs->Header.Absolute = std::max((spin + 0xff) >> 8, 0xffu);

   return KSTATUS_SUCCESS;
}

KBOOL
RtlTryEnterCriticalSection(ptr32_t<KCriticalSection> cs)
{
   int32_t unlockedValue = -1;
   auto thread = GetCurrentThread()->getPcr()->PrcbData.CurrentThread;

   bool acquired = std::atomic_compare_exchange_strong(&cs->LockCount,
                                                       &unlockedValue,
                                                       0);

   if (acquired) {
      cs->OwningThread = thread;
      cs->RecursionCount = 1;
      return KTRUE;
   } else if (cs->OwningThread == thread) {
      cs->LockCount.fetch_add(1);
      cs->RecursionCount++;
      return KTRUE;
   }

   return KFALSE;
}

void
RtlpWaitCriticalSection(ptr32_t<KCriticalSection> cs)
{
   KeWaitForSingleObject(static_cast<ptr32_t<void>>(cs), 8, 0, 0, 0);
   cs->OwningThread = GetCurrentThread()->getPcr()->PrcbData.CurrentThread;
   cs->RecursionCount = 1;
}

void
RtlpUnWaitCriticalSection(ptr32_t<KCriticalSection> cs)
{
   KeSetEvent(static_cast<ptr32_t<KEvent>>(cs), 1, 0);
}

KSTATUS
RtlEnterCriticalSection(ptr32_t<KCriticalSection> cs)
{
   auto currentThread = GetCurrentThread()->getPcr()->PrcbData.CurrentThread;
   auto spinCount = static_cast<uint64_t>(cs->Header.Absolute) << 8;

spin:
   if (cs->LockCount.fetch_add(1) != -1) {
      if (cs->OwningThread == currentThread) {
         cs->RecursionCount++;
         return KSTATUS_SUCCESS;
      }

      cs->LockCount.fetch_sub(1);

      if (spinCount) {
         --spinCount;
         goto spin;
      }

      // Out of spins, do a wait
      RtlpWaitCriticalSection(cs);
   }

   // Own it
   cs->OwningThread = currentThread;
   cs->RecursionCount = 1;
   return KSTATUS_SUCCESS;
}

KSTATUS
RtlLeaveCriticalSection(ptr32_t<KCriticalSection> cs)
{
   cs->RecursionCount--;

   if (cs->RecursionCount) {
      cs->LockCount.fetch_sub(1);
      return KSTATUS_SUCCESS;
   }

   cs->OwningThread = nullptr;

   // If lock count != -1 then there is a waiter
   if (cs->LockCount.fetch_sub(1) != 0) {
      RtlpUnWaitCriticalSection(cs);
   }

   return KSTATUS_SUCCESS;
}
