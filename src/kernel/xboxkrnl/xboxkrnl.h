#ifndef XBOX_KERNEL_H
#define XBOX_KERNEL_H

#include "kernel/kernel.h"

XBXKRNL void
EmuInitKernel(IKernel *kernel);

extern IKernel *g_kernel;

#endif
