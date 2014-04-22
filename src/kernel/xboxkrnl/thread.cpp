#include "thread.h"
#include "xboxkrnl.h"
#include "common/memory.h"

#include <Windows.h>

DWORD __stdcall ExCreateThreadStub(LPVOID lpThreadParameter)
{
   Thread *thread = reinterpret_cast<Thread *>(lpThreadParameter);
   g_kernel->resumeThread(thread);
   return 0;
}

XBXKRNL uint32_t
ExCreateThread(ptr32<uint32_t> pHandle,
               uint32_t dwStackSize,
               ptr32<uint32_t> lpThreadId,
               ptr32<void*> apiThreadStartup,
               ptr32<void*> lpStartAddress,
               ptr32<void*> lpParameter,
               uint32_t dwCreationFlagsMod)
{
   Thread *thread = new Thread();

   memset(thread, 0, sizeof(Thread));
   thread->stackSize        = dwStackSize;
   thread->apiThreadStartup = apiThreadStartup;
   thread->startAddress     = lpStartAddress;
   thread->startParameter   = lpParameter;
   thread->creationFlags    = dwCreationFlagsMod;

   auto handle = CreateThread(NULL,
                              0,
                              &ExCreateThreadStub,
                              reinterpret_cast<LPVOID>(thread),
                              0,
                              NULL);

   thread->handle = reinterpret_cast<uint32_t>(handle);

   if (pHandle) {
      Memory::write(pHandle, thread->handle);
   }

   if (lpThreadId) {
      Memory::write(lpThreadId, thread->id);
   }

   return 0;
}
