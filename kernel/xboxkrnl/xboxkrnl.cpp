#include "xboxkrnl.h"
#include "xbox.h"
#include "xex.h"

IKernel *g_kernel;

XBXKRNL XVOID
EmuInitKernel(IKernel *kernel)
{
   g_kernel = kernel;

   xexInit();
   xboxInit();
}
