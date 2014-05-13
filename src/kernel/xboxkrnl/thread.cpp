#include "kernel/xboxkrnl/thread.h"
#include "kernel/xboxkrnl/process.h"

#include "common/memory.h"

#include "system.h"

#include <atomic>
#include <thread>

std::atomic_uint32_t gThreadIdCounter = 0;
_declspec(thread) Thread *gCurrentThread;


Thread::Thread(uint32_t stackSize,
               ptr32_t<void> apiThreadStartup,
               ptr32_t<void> startAddress,
               ptr32_t<void> startParameter,
               uint32_t flags) :
   mStackSize(stackSize),
   mApiThreadStartup(apiThreadStartup),
   mStartAddress(startAddress),
   mStartParameter(startParameter),
   mCreationFlags(flags),
   mID(++gThreadIdCounter),
   mState(nullptr)
{
   mThread = std::thread(&Thread::entry, this);
}

void *Thread::getState()
{
   return mState;
}

void Thread::setState(void *state)
{
   mState = state;
}

uint32_t Thread::getId()
{
   return mID;
}

uint32_t Thread::getHandle()
{
   assert(reinterpret_cast<uint64_t>(this) < 0xffffffff);
   return reinterpret_cast<uint32_t>(this);
}

ptr32_t<void> Thread::getStackBase()
{
   return reinterpret_cast<void *>(mStack.data() + mStackSize);
}

ptr32_t<void> Thread::getStartAddress()
{
   return mStartAddress;
}

ptr32_t<void> Thread::getStartParameter()
{
   return mStartParameter;
}

ptr32_t<KPcr> Thread::getPcr()
{
   return &mPCR;
}

void Thread::join()
{
   mThread.join();
}

// Entry point to thread
void Thread::entry()
{
   auto process = KeGetCurrentProcess();

   gCurrentThread = this;

   // Allocate stack
   mStack.resize(mStackSize);

   // Allocate TLS
   mTLS.resize(process->SizeOfTlsSlots);

   // Set up kpcr
   mPCR.PrcbData.CurrentThread = &mKThread;
   mPCR.TlsData = mTLS.data() + process->SizeOfTlsSlots;
   
   // Setup kthread
   mKThread.ThreadId = mID;
   mKThread.ApcState.Process = process;

   // Execute thread on CPU
   gSystem.resumeThread(this);
}

KSTATUS
ExCreateThread(ptr32_t<be_uint32_t> pHandle,
               uint32_t dwStackSize,
               ptr32_t<be_uint32_t> lpThreadId,
               ptr32_t<void> apiThreadStartup,
               ptr32_t<void> lpStartAddress,
               ptr32_t<void> lpParameter,
               uint32_t dwCreationFlagsMod)
{
   Thread *thread = new Thread {
      dwStackSize,
      apiThreadStartup,
      lpStartAddress,
      lpParameter,
      dwCreationFlagsMod
   };

   if (pHandle) {
      *pHandle = thread->getHandle();
   }

   if (lpThreadId) {
      *lpThreadId = thread->getId();
   }

   return KSTATUS_SUCCESS;
}

Thread *GetCurrentThread()
{
   return gCurrentThread;
}
