#include "kernel/xboxkrnl/wait.h"

KNTSTATUS
KeWaitForSingleObject(ptr32_t<void> Object,
                      uint32_t WaitReason,
                      uint32_t WaitMode,
                      uint32_t Alertable,
                      ptr32_t<uint64_t> Timeout)
{
   return 0;
}
