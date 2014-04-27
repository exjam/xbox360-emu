#ifndef XBOXKRNL_WAIT_H
#define XBOXKRNL_WAIT_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

KNTSTATUS
KeWaitForSingleObject(ptr32_t<void> Object,
                      uint32_t WaitReason,
                      uint32_t WaitMode,
                      uint32_t Alertable,
                      ptr32_t<uint64_t> Timeout);

#endif // ifndef XBOXKRNL_WAIT_H
