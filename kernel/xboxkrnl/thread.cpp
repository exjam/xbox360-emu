#include "thread.h"
#include "xboxkrnl.h"

#include <util/be/memory.h>

using be::Memory;

DWORD __stdcall ExCreateThreadStub(LPVOID lpThreadParameter)
{
   Thread *thread = reinterpret_cast<Thread *>(lpThreadParameter);
   g_kernel->resumeThread(thread);
   return 0;
}

XBXKRNL XDWORD
ExCreateThread(XLPHANDLE pHandle,
               XDWORD dwStackSize,
               XLPDWORD lpThreadId,
               XLPVOID apiThreadStartup,
               XLPVOID lpStartAddress,
               XLPVOID lpParameter,
               XDWORD dwCreationFlagsMod)
{
   Thread *thread = new Thread();

   memset(thread, 0, sizeof(Thread));
   thread->stackSize        = dwStackSize;
   thread->apiThreadStartup = apiThreadStartup;
   thread->startAddress     = lpStartAddress;
   thread->startParameter   = lpParameter;
   thread->creationFlags    = dwCreationFlagsMod;

   thread->handle = CreateThread(NULL,
                                 0,
                                 &ExCreateThreadStub,
                                 reinterpret_cast<LPVOID>(thread),
                                 0,
                                 NULL);

   if (thread) {
      if (pHandle) {
         Memory::write(pHandle, thread->handle);
      }

      if (lpThreadId) {
         Memory::write(lpThreadId, thread->id);
      }
   }

   return 0;
}
