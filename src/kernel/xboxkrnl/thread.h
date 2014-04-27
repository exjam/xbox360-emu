#ifndef XBOXKRNL_THREAD_H
#define XBOXKRNL_THREAD_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

#include <vector>
#include <thread>

class Thread
{
public:
   Thread(uint32_t stackSize,
          ptr32_t<void> apiThreadStartup,
          ptr32_t<void> startAddress,
          ptr32_t<void> startParameter,
          uint32_t flags);

   uint32_t getId();
   uint32_t getHandle();

   void *getState();
   void setState(void *state);

   ptr32_t<void> getStackBase();
   ptr32_t<void> getStartAddress();
   ptr32_t<void> getStartParameter();

   ptr32_t<KPcr> getPcr();

   void join();

private:
   void entry();

private:
   uint32_t mID;

   uint32_t mStackSize;
   uint32_t mCreationFlags;
   ptr32_t<void> mApiThreadStartup;
   ptr32_t<void> mStartAddress;
   ptr32_t<void> mStartParameter;
   std::thread mThread;

   void *mState; // State used by CPU interpreter
   KPcr mPCR;    // Structure in r13
   KThread mKThread;
   std::vector<uint8_t> mStack;
   std::vector<uint8_t> mTLS;
};

uint32_t
ExCreateThread(ptr32_t<be_uint32_t> pHandle,
               uint32_t dwStackSize,
               ptr32_t<be_uint32_t> lpThreadId,
               ptr32_t<void> apiThreadStartup,
               ptr32_t<void> lpStartAddress,
               ptr32_t<void> lpParameter,
               uint32_t dwCreationFlagsMod);

#endif // ifndef XBOXKRNL_THREAD_H
