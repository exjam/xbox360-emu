#ifndef XBXKRNL_KE_H
#define XBXKRNL_KE_H

#include "kernel/kernel.h"
#include "nt.h"

XBXKRNL uint32_t
KeDebugMonitorData;

XBXKRNL uint8_t
KeGetCurrentProcessType();

XBXKRNL uint64_t
KeTlsAlloc();

XBXKRNL uint64_t
KeTlsFree(uint32_t dwTlsIndex);

XBXKRNL uint32_t
KeTlsGetValue(uint32_t dwTlsIndex);

XBXKRNL uint64_t
KeTlsSetValue(uint32_t dwTlsIndex, uint32_t lpTlsValue);

XBXKRNL XNTSTATUS
KeWaitForSingleObject(ptr32<void> Object, uint32_t WaitReason, uint32_t WaitMode, uint32_t Alertable, ptr32<uint64_t> Timeout);

#endif // ifndef XBXKRNL_KE_H
