#ifndef XBOXKRNL_CRITICAL_SECTION_H
#define XBOXKRNL_CRITICAL_SECTION_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

KSTATUS
RtlInitializeCriticalSection(ptr32_t<KCriticalSection> cs);

KSTATUS
RtlInitializeCriticalSectionAndSpinCount(ptr32_t<KCriticalSection> cs,
                                         uint32_t spin);

KBOOL
RtlTryEnterCriticalSection(ptr32_t<KCriticalSection> cs);

KSTATUS
RtlEnterCriticalSection(ptr32_t<KCriticalSection> cs);

KSTATUS
RtlLeaveCriticalSection(ptr32_t<KCriticalSection> cs);

#endif // ifndef XBOXKRNL_CRITICAL_SECTION_H
