#include "nt.h"
#include "util/be/memory.h"
#include "util/log.h"

XBXKRNL NTSTATUS
NtAllocateVirtualMemory(PVOID *pBaseAddress, PSIZE_T pRegionSize, ULONG AllocationType, ULONG Protect, ULONG Flags)
{
   uint32_t RegionSize = 0;
   PVOID BaseAddress = 0;

   if (pRegionSize) {
      RegionSize = be::Memory::read<uint32_t>(reinterpret_cast<uint64_t>(pRegionSize));
   }

   if (pBaseAddress) {
      BaseAddress = reinterpret_cast<PVOID>(be::Memory::read<uint32_t>(reinterpret_cast<uint64_t>(pBaseAddress)));
   }

   BaseAddress = be::Memory::translate(BaseAddress);

   xDebug()
      << "NtAllocateVirtualMemory("
      << Log::hex(reinterpret_cast<uint64_t>(BaseAddress)) << ", "
      << Log::hex(RegionSize) << ", "
      << Log::hex(AllocationType) << ", "
      << Log::hex(Protect) << ", "
      << Log::hex(Flags) << ")";

   auto va = VirtualAlloc(BaseAddress, RegionSize, AllocationType & 0xFFFF, Protect);

   if (va) {
      be::Memory::write<uint32_t>(reinterpret_cast<uint64_t>(pBaseAddress), reinterpret_cast<uint32_t>(va));
   }

   return va != 0;
}