#ifndef XBOXKRNL_SPINLOCK_H
#define XBOXKRNL_SPINLOCK_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

uint64_t
KfAcquireSpinLock(KSpinLock lock);

#endif
