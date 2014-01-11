#include "nt.h"

#include <util/log.h>
#include <winternl.h>
#include <Windows.h>

XBXKRNL XNTSTATUS
NtAllocateVirtualMemory(XLPDWORD lpBaseAddress,
                        XLPDWORD lpRegionSize,
                        XDWORD dwAllocationType,
                        XDWORD dwProtect,
                        XDWORD dwFlags)
{
   XDWORD RegionSize = 0;
   XDWORD BaseAddress = 0;
   XDWORD result;

   swapField(lpRegionSize, &RegionSize);
   swapField(lpBaseAddress, &BaseAddress);
   
   xDebug()
      << "NtAllocateVirtualMemory("
      << Log::hex(BaseAddress) << ", "
      << Log::hex(RegionSize) << ", "
      << Log::hex(dwAllocationType) << ", "
      << Log::hex(dwProtect) << ", "
      << Log::hex(dwFlags) << ")";

   result = reinterpret_cast<XDWORD>(
               VirtualAlloc(reinterpret_cast<LPVOID>(BaseAddress),
                           RegionSize,
                           dwAllocationType & 0xFFFF,
                           dwProtect));

   swapField(&result, lpBaseAddress);

   return (result != 0) ? XSTATUS_SUCCESS : XSTATUS_NO_MEMORY;
}