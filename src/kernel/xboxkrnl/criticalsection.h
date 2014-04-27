#ifndef XBOXKRNL_CRITICAL_SECTION_H
#define XBOXKRNL_CRITICAL_SECTION_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

void
RtlInitializeCriticalSection(ptr32_t<KCriticalSection> cs);

void
RtlInitializeCriticalSectionAndSpinCount(ptr32_t<KCriticalSection> cs,
                                         uint32_t spin);

uint64_t
RtlTryEnterCriticalSection(ptr32_t<KCriticalSection> cs);

void
RtlEnterCriticalSection(ptr32_t<KCriticalSection> cs);

void
RtlLeaveCriticalSection(ptr32_t<KCriticalSection> cs);

#endif // ifndef XBOXKRNL_CRITICAL_SECTION_H
