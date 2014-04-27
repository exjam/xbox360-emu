#include "kernel/xboxkrnl/process.h"
#include "common/memory.h"
#include "xex/xex.h"
#include "system.h"
#include <memory>

std::unique_ptr<KProcess> gProcess;

uint8_t
KeGetCurrentProcessType()
{
   return static_cast<uint8_t>(ProcessType::User);
}

KProcess *
KeGetCurrentProcess()
{
   return gProcess.get();
}

KProcess *
KeInitializeProcess()
{
   gProcess = std::unique_ptr<KProcess> { new KProcess() };

   // Initialize TLS bitmap
   auto binary = gSystem.getBinary();
   auto slotSize = binary->header.tlsInfo.dataSize * binary->header.tlsInfo.slotCount;

   gProcess->SizeOfTlsSlots = slotSize;
   memset(gProcess->TlsSlotBitmap, 0xff, 8 * 4);

   return gProcess.get();
}
