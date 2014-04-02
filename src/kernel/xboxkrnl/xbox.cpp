#include "xbox.h"
#include "common/memory.h"
#include <string.h>

XBOX_HARDWARE_INFO XboxHardwareInfo;
KERNEL_VERSION XboxKrnlVersion;
KERNEL_VERSION XboxKrnlBaseVersion;

void xboxInit()
{
   memset(&XboxHardwareInfo, 0, sizeof(XBOX_HARDWARE_INFO));
   XboxHardwareInfo.Flags       = 0x813;
   
   memset(&XboxKrnlVersion,  0, sizeof(KERNEL_VERSION));
   XboxKrnlVersion.Major        = 2;
   XboxKrnlVersion.Minor        = 0;
   XboxKrnlVersion.Build        = 14448;
   XboxKrnlVersion.ApprovalType = 0x80;
   XboxKrnlVersion.QFE          = 0;

   memcpy(&XboxKrnlBaseVersion, &XboxKrnlVersion, sizeof(KERNEL_VERSION));
}
