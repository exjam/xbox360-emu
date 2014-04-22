#ifndef XBXKRNL_RTL_H
#define XBXKRNL_RTL_H

#include "kernel/kernel.h"

XBXKRNL void
RtlInitializeCriticalSection(ptr32<KCriticalSection> lpCriticalSection);

XBXKRNL void
RtlInitializeCriticalSectionAndSpinCount(ptr32<KCriticalSection> lpCriticalSection,
                                         uint32_t dwSpinCount);

XBXKRNL uint64_t
RtlTryEnterCriticalSection(ptr32<KCriticalSection> lpCriticalSection);

XBXKRNL void
RtlEnterCriticalSection(ptr32<KCriticalSection> lpCriticalSection);

XBXKRNL void
RtlLeaveCriticalSection(ptr32<KCriticalSection> lpCriticalSection);

#endif // ifndef XBXKRNL_RTL_H
