#ifndef XBOX_H
#define XBOX_H

#include "kernel/kernel.h"
#include "common/betype.h"

#pragma pack(push, 1)

struct XBOX_HARDWARE_INFO
{
   BE<XDWORD> Flags;
   BE<XBYTE>  NumberOfProcessors;
   BE<XBYTE>  PCIBridgeRevisionID;
   BE<XBYTE>  Reserved[6];
   BE<XWORD>  BldrMagic;
   BE<XWORD>  BldrFlags;
};

struct KERNEL_VERSION
{
   BE<XWORD> Major;
   BE<XWORD> Minor;
   BE<XWORD> Build;
   BE<XBYTE> ApprovalType;
   BE<XBYTE> QFE;
};

#pragma pack(pop)

XBXKRNL XBOX_HARDWARE_INFO XboxHardwareInfo;

XBXKRNL KERNEL_VERSION XboxKrnlVersion;
XBXKRNL KERNEL_VERSION XboxKrnlBaseVersion;

void xboxInit();

#endif
