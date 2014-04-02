#include "ke.h"

#include <Windows.h>

XDWORD KeDebugMonitorData = 0;

enum class ProcessType
{
   Idle = 0,
   User = 1,
   System = 2
};

XBXKRNL XBYTE
KeGetCurrentProcessType()
{
   return static_cast<XBYTE>(ProcessType::User);
}

XBXKRNL XDWORD
KeTlsAlloc()
{
   return TlsAlloc();
}

XBXKRNL XBOOL
KeTlsFree(XDWORD dwTlsIndex)
{
   return TlsFree(dwTlsIndex);
}

XBXKRNL XPVOID
KeTlsGetValue(XDWORD dwTlsIndex)
{
   return TlsGetValue(dwTlsIndex);
}

XBXKRNL XBOOL
KeTlsSetValue(XDWORD dwTlsIndex, 
              XLPVOID lpTlsValue)
{
   return TlsSetValue(dwTlsIndex, lpTlsValue);
}
