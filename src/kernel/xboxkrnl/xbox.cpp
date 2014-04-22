#include "xbox.h"
#include "common/memory.h"
#include <string.h>

HardwareInfo XboxHardwareInfo;
KernelVersion XboxKrnlVersion;
KernelVersion XboxKrnlBaseVersion;

void xboxInit()
{
   memset(&XboxHardwareInfo, 0, sizeof(HardwareInfo));
   beSet(XboxHardwareInfo.Flags, 0x813);
   
   memset(&XboxKrnlVersion, 0, sizeof(KernelVersion));
   beSet(XboxKrnlVersion.Major, 2);
   beSet(XboxKrnlVersion.Minor, 0);
   beSet(XboxKrnlVersion.Build, 14448);
   beSet(XboxKrnlVersion.ApprovalType, 0x80);
   beSet(XboxKrnlVersion.QFE, 0);

   memcpy(&XboxKrnlBaseVersion, &XboxKrnlVersion, sizeof(KernelVersion));
}
