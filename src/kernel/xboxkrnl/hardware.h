#ifndef XBOXKRNL_HARDWARE_H
#define XBOXKRNL_HARDWARE_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "common/bigendian.h"

#pragma pack(push, 1)

struct HardwareInfo
{
   be_uint32_t Flags;
   be_uint8_t NumberOfProcessors;
   be_uint8_t PCIBridgeRevisionID;
   be_uint8_t Reserved[6];
   be_uint16_t BldrMagic;
   be_uint16_t BldrFlags;
};

CHECK_SIZE(HardwareInfo, 0x10);

#pragma pack(pop)

extern HardwareInfo
XboxHardwareInfo;

void xboxkrnlHardwareInit();

#endif // ifndef XBOXKRNL_HARDWARE_H
