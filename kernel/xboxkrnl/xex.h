#ifndef XEX_H
#define XEX_H

#include "kernel.h"
#include <winternl.h>

XBXKRNL void* XexExecutableModuleHandle;

XBXKRNL uint32_t
XexGetModuleHandle(char* module, void* hand);

#endif
