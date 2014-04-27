#ifndef XBOXKRNL_PROCESS_H
#define XBOXKRNL_PROCESS_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

enum class ProcessType
{
   Idle = 0,
   User = 1,
   System = 2
};

uint8_t
KeGetCurrentProcessType();

KProcess *
KeGetCurrentProcess();

KProcess *
KeInitializeProcess();

#endif // XBOXKRNL_PROCESS_H
