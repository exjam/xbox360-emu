#ifndef XBOX_H
#define XBOX_H

#include <kernel.h>

#pragma pack(push, 1)

struct XBOX_HARDWARE_INFO
{
   XDWORD Flags;
   XBYTE  NumberOfProcessors;
   XBYTE  PCIBridgeRevisionID;
   XBYTE  Reserved[6];
   XWORD  BldrMagic;
   XWORD  BldrFlags;
};

struct KERNEL_VERSION
{
   XWORD Major;
   XWORD Minor;
   XWORD Build;
   XBYTE ApprovalType;
   XBYTE QFE;
};

#pragma pack(pop)

XBXKRNL XBOX_HARDWARE_INFO XboxHardwareInfo;

XBXKRNL KERNEL_VERSION XboxKrnlVersion;
XBXKRNL KERNEL_VERSION XboxKrnlBaseVersion;

void xboxInit();

#endif
