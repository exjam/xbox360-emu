#ifndef XBOXKRNL_OBJECT_H
#define XBOXKRNL_OBJECT_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "kernel/xboxkrnl/ntstructs.h"

class Waitable
{
public:
   enum WaitStatus
   {
      Success,
      Timeout,
      Failure
   };

   virtual WaitStatus wait(uint32_t timeoutMS) = 0;
};

class Object
{
public:
   enum Type
   {
      Event
   };

protected:
   Object(Type type) :
      mType(type)
   {
   }

public:
   Type getType()
   {
      return mType;
   }

   virtual Waitable *getWaitable()
   {
      return nullptr;
   }

private:
   Type mType;
};

Object *
GetKernelObject(KDispatcherHeader *object);

uint64_t
KeSetEvent(ptr32_t<KEvent> object,
           uint32_t increment,
           uint32_t wait);

KSTATUS
KeWaitForSingleObject(ptr32_t<void> Object,
                      uint32_t WaitReason,
                      uint32_t WaitMode,
                      uint32_t Alertable,
                      ptr32_t<uint64_t> Timeout);

#endif // ifndef XBOXKRNL_OBJECT_H
