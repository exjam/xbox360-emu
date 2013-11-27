#include "xbox.h"
#include <string.h>
#include <util/be/memory.h>

using be::Memory;

XBOX_HARDWARE_INFO XboxHardwareInfo;
KERNEL_VERSION XboxKrnlVersion;
KERNEL_VERSION XboxKrnlBaseVersion;

void xboxInit()
{
   memset(&XboxHardwareInfo, 0, sizeof(XBOX_HARDWARE_INFO));
   XboxHardwareInfo.Flags = bits::swap<XDWORD>(0x813);
   
   memset(&XboxKrnlVersion, 0, sizeof(KERNEL_VERSION));
   XboxKrnlVersion.Major        = bits::swap<XWORD>(2);
   XboxKrnlVersion.Minor        = bits::swap<XWORD>(0);
   XboxKrnlVersion.Build        = bits::swap<XWORD>(14448);
   XboxKrnlVersion.ApprovalType = bits::swap<XBYTE>(0x80);
   XboxKrnlVersion.QFE          = bits::swap<XBYTE>(0);

   memcpy(&XboxKrnlBaseVersion, &XboxKrnlVersion, sizeof(KERNEL_VERSION));
}
