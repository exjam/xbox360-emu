#include "nt.h"
#include "common/log.h"

#include <winternl.h>
#include <Windows.h>

XBXKRNL XNTSTATUS
NtAllocateVirtualMemory(ptr32<uint32_t> lpBaseAddress,
                        ptr32<uint32_t> lpRegionSize,
                        uint32_t dwAllocationType,
                        uint32_t dwProtect,
                        uint32_t dwFlags)
{
   uint32_t RegionSize = 0;
   uint32_t BaseAddress = 0;

   if (lpBaseAddress) {
      BaseAddress = bits::swap(*lpBaseAddress);
   }

   if (lpRegionSize) {
      RegionSize = bits::swap(*lpRegionSize);
   }
   
   xDebug()
      << "NtAllocateVirtualMemory("
      << Log::hex(BaseAddress) << ", "
      << Log::hex(RegionSize) << ", "
      << Log::hex(dwAllocationType) << ", "
      << Log::hex(dwProtect) << ", "
      << Log::hex(dwFlags) << ")";

   BaseAddress = reinterpret_cast<uint32_t>(VirtualAlloc(reinterpret_cast<LPVOID>(BaseAddress),
                                                         RegionSize,
                                                         dwAllocationType & 0xFFFF,
                                                         dwProtect));

   if (lpBaseAddress) {
      *lpBaseAddress = bits::swap(BaseAddress);
   }

   return (BaseAddress != 0) ? XSTATUS_SUCCESS : XSTATUS_NO_MEMORY;
}
