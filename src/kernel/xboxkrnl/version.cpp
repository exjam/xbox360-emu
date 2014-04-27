#include "kernel/xboxkrnl/version.h"
#include "common/memory.h"

KernelVersion XboxKrnlVersion;
KernelVersion XboxKrnlBaseVersion;

void xboxkrnlVersionInit()
{
   memset(&XboxKrnlVersion, 0, sizeof(KernelVersion));
   XboxKrnlVersion.Major = 2;
   XboxKrnlVersion.Minor = 0;
   XboxKrnlVersion.Build = 14448;
   XboxKrnlVersion.ApprovalType = 0x80;
   XboxKrnlVersion.QFE = 0;

   memset(&XboxKrnlBaseVersion, 0, sizeof(KernelVersion));
   XboxKrnlBaseVersion.Major = 2;
   XboxKrnlBaseVersion.Minor = 0;
   XboxKrnlBaseVersion.Build = 14448;
   XboxKrnlBaseVersion.ApprovalType = 0x80;
   XboxKrnlBaseVersion.QFE = 0;
}
