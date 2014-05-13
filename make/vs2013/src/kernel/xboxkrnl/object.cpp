#include "kernel/xboxkrnl/object.h"
#include "kernel/xboxkrnl/event.h"

Object *
InitializeObject(KDispatcherHeader *header)
{
   switch (header->Type) {
   case EventNotificationObject:
      return new Event(Event::ManualReset, !!header->SignalState);
   case EventSynchronizationObject:
      return new Event(Event::AutoReset, !!header->SignalState);
   default:
      assert(0);
      return nullptr;
   }
}

Object *
GetKernelObject(void *object)
{
   KDispatcherHeader *header = reinterpret_cast<KDispatcherHeader*>(object);

   if (header->WaitList.object == nullptr) {
      return InitializeObject(header);
   } else {
      return header->WaitList.object;
   }
}

KBOOL
KeSetEvent(ptr32_t<KEvent> event,
           uint32_t increment,
           uint32_t wait)
{
   auto object = GetKernelObject(event.getPointer());

   assert(object->getType() == Object::Event);

   return reinterpret_cast<Event*>(object)->set();
}

KSTATUS
KeWaitForSingleObject(ptr32_t<void> Object,
                      uint32_t WaitReason,
                      uint32_t WaitMode,
                      uint32_t Alertable,
                      ptr32_t<uint64_t> Timeout)
{
   uint64_t timeoutTicks = 0;

   if (Timeout) {
      timeoutTicks = *Timeout;
   }

   assert(timeoutTicks == 0);

   auto kernelObject = GetKernelObject(Object.getPointer());
   auto waitable = kernelObject->getWaitable();
   auto status = waitable->wait(0);

   switch (status) {
   case Waitable::Timeout:
      return KSTATUS_TIMEOUT;
   case Waitable::Failure:
      return KSTATUS_ABANDONED_WAIT;
   default:
      return KSTATUS_SUCCESS;
   }
}
