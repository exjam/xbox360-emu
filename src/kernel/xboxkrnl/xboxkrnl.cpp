#include "kernel/xboxkrnl/xboxkrnl.h"

#include "kernel/xboxkrnl/criticalsection.h"
#include "kernel/xboxkrnl/debug.h"
#include "kernel/xboxkrnl/hardware.h"
#include "kernel/xboxkrnl/process.h"
#include "kernel/xboxkrnl/spinlock.h"
#include "kernel/xboxkrnl/thread.h"
#include "kernel/xboxkrnl/tls.h"
#include "kernel/xboxkrnl/version.h"
#include "kernel/xboxkrnl/virtualmemory.h"
#include "kernel/xboxkrnl/object.h"
#include "kernel/xboxkrnl/xex.h"

#include "kernel/module.h"
#include "system.h"

xboxkrnl::xboxkrnl() :
   Module("xboxkrnl.exe")
{
   // Register exports
   addExports();

   // Init any required submodules
   xboxkrnlXexInit();
   xboxkrnlVersionInit();
   xboxkrnlHardwareInit();
}

// Export ordinals
void xboxkrnl::addExports()
{
   exportFunction(13, ExCreateThread);
   //exportVariable(14, ExEventObjectType);

   //exportVariable(27, ExThreadObjectType);

   exportVariable(89, KeDebugMonitorData);

   exportFunction(102, KeGetCurrentProcessType);

   //exportVariable(173, KeTimeStampBundle);

   exportFunction(157, KeSetEvent);

   exportFunction(176, KeWaitForSingleObject);
   exportFunction(177, KfAcquireSpinLock);

   exportFunction(204, NtAllocateVirtualMemory);

   exportFunction(338, KeTlsAlloc);
   exportFunction(339, KeTlsFree);
   exportFunction(340, KeTlsGetValue);
   exportFunction(341, KeTlsSetValue);

   exportVariable(342, XboxHardwareInfo);
   exportVariable(343, XboxKrnlBaseVersion);
   exportVariable(344, XboxKrnlVersion);

   exportFunction(293, RtlEnterCriticalSection);

   exportFunction(299, RtlImageXexHeaderField);

   exportFunction(302, RtlInitializeCriticalSection);
   exportFunction(303, RtlInitializeCriticalSectionAndSpinCount);
   exportFunction(304, RtlLeaveCriticalSection);

   exportFunction(321, RtlTryEnterCriticalSection);

   exportVariable(403, XexExecutableModuleHandle);
   exportFunction(404, XexCheckExecutablePrivilege);
   exportFunction(405, XexGetModuleHandle);

   //exportVariable(430, ExLoadedCommandLine);

   //exportVariable(446, VdGlobalDevice);
   //exportVariable(447, VdGlobalXamDevice);
   //exportVariable(448, VdGpuClockInMHz);
   //exportVariable(449, VdHSIOCalibrationLock);

   //exportVariable(614, KeCertMonitorData);
}
