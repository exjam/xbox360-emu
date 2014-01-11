#include "rtl.h"

#include <Windows.h>
#include <util/memory.h>

XBXKRNL XVOID
RtlInitializeCriticalSection(XLPCRITICAL_SECTION lpCriticalSection)
{
   if (lpCriticalSection->Synchronization.Magic == 0x1BADBEEF) {
      return;
   }

   memset(lpCriticalSection, 0xdd, sizeof(XCRITICAL_SECTION));

   lpCriticalSection->Synchronization.Magic = 0x1BADBEEF;
   lpCriticalSection->Synchronization.RealCriticalSection = new RTL_CRITICAL_SECTION();
   InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(lpCriticalSection->Synchronization.RealCriticalSection));
}

XBXKRNL XVOID
RtlInitializeCriticalSectionAndSpinCount(XLPCRITICAL_SECTION lpCriticalSection,
                                         XDWORD dwSpinCount)
{
   if (lpCriticalSection->Synchronization.Magic == 0x1BADBEEF) {
      return;
   }

   memset(lpCriticalSection, 0xdd, sizeof(XCRITICAL_SECTION));

   lpCriticalSection->Synchronization.Magic = 0x1BADBEEF;
   lpCriticalSection->Synchronization.RealCriticalSection = new RTL_CRITICAL_SECTION();
   InitializeCriticalSectionAndSpinCount(reinterpret_cast<LPCRITICAL_SECTION>(lpCriticalSection->Synchronization.RealCriticalSection), dwSpinCount);
}

XBXKRNL XBOOL
RtlTryEnterCriticalSection(XLPCRITICAL_SECTION lpCriticalSection)
{
   if (lpCriticalSection->Synchronization.Magic != 0x1BADBEEF) {
      RtlInitializeCriticalSection(lpCriticalSection);
   }

   return TryEnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(lpCriticalSection->Synchronization.RealCriticalSection));
}

XBXKRNL XVOID
RtlEnterCriticalSection(XLPCRITICAL_SECTION lpCriticalSection)
{
   if (lpCriticalSection->Synchronization.Magic != 0x1BADBEEF) {
      RtlInitializeCriticalSection(lpCriticalSection);
   }

   EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(lpCriticalSection->Synchronization.RealCriticalSection));
}

XBXKRNL XVOID
RtlLeaveCriticalSection(XLPCRITICAL_SECTION lpCriticalSection)
{
   assert(lpCriticalSection->Synchronization.Magic == 0x1BADBEEF);
   LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(lpCriticalSection->Synchronization.RealCriticalSection));
}
