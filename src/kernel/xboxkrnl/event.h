#ifndef XBOXKRNL_EVENT_H
#define XBOXKRNL_EVENT_H

#include "object.h"
#include <condition_variable>

class Event : public Object, public Waitable
{
public:
   enum ResetType
   {
      AutoReset,
      ManualReset
   };

public:
   Event(ResetType resetType, bool initialState) :
      Object(Object::Event),
      mResetType(resetType)
   {
      mEvent.store(initialState);
   }

   bool set()
   {
      bool previous = mEvent.exchange(true);

      // Auto-reset should only wake one thread and then immediately reset event
      if (mResetType == AutoReset) {
         mConVar.notify_one();
      } else {
         mConVar.notify_all();
      }

      return previous;
   }

   void reset()
   {
      mEvent.store(false);
   }

   Waitable *getWaitable() override
   {
      return this;
   }

   Waitable::WaitStatus wait(uint32_t milliseconds) override
   {
      bool status = true;

      // Immediately return if event is already set
      if (mEvent.load()) {
         return Waitable::Success;
      }

      // Wait for event to be set
      std::unique_lock<std::mutex> lock { mMutex };

      if (!milliseconds) {
         mConVar.wait(lock, [this]{ return mEvent.load(); });
      } else {
         status = mConVar.wait_for(lock,
                                   std::chrono::milliseconds { milliseconds },
                                   [this]{ return mEvent.load(); });
      }

      // Reset if needed
      if (status && mResetType == AutoReset) {
         reset();
      }

      return status ? Waitable::Success : Waitable::Timeout;
   }

private:
   ResetType mResetType;
   std::mutex mMutex;
   std::atomic_bool mEvent;
   std::condition_variable mConVar;
};

#endif
