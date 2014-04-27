#include "kernel/xboxkrnl/hardware.h"
#include "common/memory.h"

HardwareInfo XboxHardwareInfo;

void xboxkrnlHardwareInit()
{
   memset(&XboxHardwareInfo, 0, sizeof(HardwareInfo));
   XboxHardwareInfo.Flags = 0x813;
}
