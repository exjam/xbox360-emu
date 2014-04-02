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

/*
#define KernelModuleBegin(name) void test() {
#define KernelModuleEnd() }

template<typename ReturnType, typename... Args>
void defineKernelExport(int ordinal, std::function<ReturnType(Args...)> &func)
{
}
*/