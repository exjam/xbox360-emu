#ifndef XBOXKRNL_VERSION_H
#define XBOXKRNL_VERSION_H

#include "kernel/xboxkrnl/xboxkrnl.h"
#include "common/bigendian.h"

#pragma pack(push, 1)

struct KernelVersion
{
   be_uint16_t Major;
   be_uint16_t Minor;
   be_uint16_t Build;
   be_uint8_t ApprovalType;
   be_uint8_t QFE;
};

CHECK_SIZE(KernelVersion, 0x8);

#pragma pack(pop)

extern KernelVersion
XboxKrnlVersion;

extern KernelVersion
XboxKrnlBaseVersion;

void xboxkrnlVersionInit();

#endif // ifndef XBOXKRNL_VERSION_H
