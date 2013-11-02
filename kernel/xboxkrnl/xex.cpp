#include "xex.h"

#include <Windows.h>
#include "util/be/memory.h"

void* XexExecutableModuleHandle;

XBXKRNL uint32_t XexGetModuleHandle(char* module, void* hand)
{
   HMODULE handle = GetModuleHandleA(module);

   be::Memory::write<uint32_t>(reinterpret_cast<uint64_t>(hand), reinterpret_cast<uint32_t>(handle));

   return (handle != 0) ? 0 : -1;
}
