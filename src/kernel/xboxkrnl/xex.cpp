#include "kernel/xboxkrnl/xex.h"
#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/process.h"
#include "common/memory.h"
#include "xex/xex.h"
#include "system.h"

be_ptr32_t<KLdrDataTableEntry> XexExecutableModuleHandle = nullptr;

uint32_t
XexGetModuleHandle(ptr32_t<char> lpModuleName,
                   ptr32_t<uint32_t> lpHandle)
{
   return -1;
}

uint64_t
XexCheckExecutablePrivilege(uint32_t priviledge)
{
   auto flags = gSystem.getBinary()->header.systemFlags.flags;
   return (flags >> priviledge) & 1;
}

ptr32_t<void>
RtlImageXexHeaderField(ptr32_t<xex::Binary> binary,
                       uint32_t key)
{
   for (auto &raw : binary->header.optionalRaw) {
      if (raw.key == key) {
         return raw.data.data();
      }
   }

   return 0;
}

void xboxkrnlXexInit()
{
   if (XexExecutableModuleHandle) {
      delete XexExecutableModuleHandle;
   }

   auto module = new KLdrDataTableEntry();
   module->XexHeaderBase = gSystem.getBinary();
   XexExecutableModuleHandle = module;
}
