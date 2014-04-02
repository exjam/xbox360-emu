#ifndef XBXKRNL_RTL_H
#define XBXKRNL_RTL_H

#include "kernel/kernel.h"

#pragma pack(push, 1)

struct XCRITICAL_SECTION
{
   /* Used for blocking */
   union
   {
      XDWORD RawEvent[4];

      struct
      {
         XDWORD Magic;
         struct _RTL_CRITICAL_SECTION *RealCriticalSection;
      };
   } Synchronization;

   /* Control Entering / Exit for resource */
   XDWORD LockCount;
   XDWORD RecursionCount;
   XHANDLE OwningThread;
};

using XLPCRITICAL_SECTION = XLP<XCRITICAL_SECTION>;

#pragma pack(pop)

XBXKRNL XVOID
RtlInitializeCriticalSection(XLPCRITICAL_SECTION lpCriticalSection);

XBXKRNL XVOID
RtlInitializeCriticalSectionAndSpinCount(XLPCRITICAL_SECTION lpCriticalSection,
                                         XDWORD dwSpinCount);

XBXKRNL XBOOL
RtlTryEnterCriticalSection(XLPCRITICAL_SECTION lpCriticalSection);

XBXKRNL XVOID
RtlEnterCriticalSection(XLPCRITICAL_SECTION lpCriticalSection);

XBXKRNL XVOID
RtlLeaveCriticalSection(XLPCRITICAL_SECTION lpCriticalSection);

#endif // ifndef XBXKRNL_RTL_H
