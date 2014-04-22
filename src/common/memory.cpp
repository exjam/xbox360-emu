#include "memory.h"
#include <windows.h>

uint8_t* Memory::allocate(uint64_t address, unsigned size)
{
   return reinterpret_cast<uint8_t*>(VirtualAlloc(reinterpret_cast<LPVOID>(address), size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
}

void Memory::free(uint64_t address, unsigned size)
{
   VirtualFree(reinterpret_cast<LPVOID>(address), size, MEM_RELEASE);
   VirtualFree(reinterpret_cast<LPVOID>(address), size, MEM_DECOMMIT);
}
