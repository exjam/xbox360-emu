#include "kernel/xboxkrnl/virtualmemory.h"

#include "common/log.h"
#include "common/memory.h"

#include <Windows.h>

KNTSTATUS
NtAllocateVirtualMemory(ptr32_t<be_uint32_t> lpBaseAddress,
                        ptr32_t<be_uint32_t> lpRegionSize,
                        uint32_t dwAllocationType,
                        uint32_t dwProtect,
                        uint32_t dwFlags)
{
   uint32_t RegionSize = 0;
   uint32_t BaseAddress = 0;
   uint64_t result;

   if (lpBaseAddress) {
      BaseAddress = *lpBaseAddress;
   }

   if (lpRegionSize) {
      RegionSize = *lpRegionSize;
   }
   
   xDebug()
      << "NtAllocateVirtualMemory("
      << Log::hex(BaseAddress) << ", "
      << Log::hex(RegionSize) << ", "
      << Log::hex(dwAllocationType) << ", "
      << Log::hex(dwProtect) << ", "
      << Log::hex(dwFlags) << ")";

   result = reinterpret_cast<uint32_t>(VirtualAlloc(reinterpret_cast<LPVOID>(BaseAddress),
                                                    RegionSize,
                                                    dwAllocationType & 0xFFFF,
                                                    dwProtect));

   assert(result < 0xFFFFFFFFull);

   if (lpBaseAddress) {
      *lpBaseAddress = static_cast<uint32_t>(result);
   }

   return (result != 0) ? KSTATUS_SUCCESS : KSTATUS_NO_MEMORY;
}
