#ifndef XBXKRNL_XBOX_H
#define XBXKRNL_XBOX_H

#include "kernel/kernel.h"

#pragma pack(push, 1)

struct HardwareInfo
{
   uint32_t Flags;
   uint8_t NumberOfProcessors;
   uint8_t PCIBridgeRevisionID;
   uint8_t Reserved[6];
   uint16_t BldrMagic;
   uint16_t BldrFlags;
};
CHECK_SIZE(HardwareInfo, 0x10);

struct KernelVersion
{
   uint16_t Major;
   uint16_t Minor;
   uint16_t Build;
   uint8_t ApprovalType;
   uint8_t QFE;
};

CHECK_SIZE(KernelVersion, 0x8);

#pragma pack(pop)

XBXKRNL HardwareInfo
XboxHardwareInfo;

XBXKRNL KernelVersion
XboxKrnlVersion;

XBXKRNL KernelVersion
XboxKrnlBaseVersion;

void xboxInit();

#endif // ifndef XBXKRNL_XBOX_H
