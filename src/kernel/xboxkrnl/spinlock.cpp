#include "kernel/xboxkrnl/spinlock.h"

uint64_t
KfAcquireSpinLock(KSpinLock lock)
{
   // Raise Irql to 2
   // Spin on lock
   return 0;
}
