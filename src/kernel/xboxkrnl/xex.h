#ifndef XBOXKRNL_XEX_H
#define XBOXKRNL_XEX_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

namespace xex
{
   struct Binary;
};

void xboxkrnlXexInit();

extern be_ptr32_t<KLdrDataTableEntry>
XexExecutableModuleHandle;

uint32_t
XexGetModuleHandle(ptr32_t<char> lpModuleName,
                   ptr32_t<uint32_t> lpHandle);
uint64_t
XexCheckExecutablePrivilege(uint32_t priviledge);

void*
RtlImageXexHeaderField(ptr32_t<xex::Binary> binary,
                       uint32_t key);

#endif // ifndef XBOXKRNL_XEX_H
