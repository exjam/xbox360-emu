#ifndef KE_H
#define KE_H

#include <kernel.h>


XBXKRNL XDWORD KeDebugMonitorData;

XBXKRNL XBYTE KeGetCurrentProcessType();

XBXKRNL XDWORD KeTlsAlloc();
XBXKRNL XBOOL KeTlsFree(XDWORD dwTlsIndex);
XBXKRNL XPVOID KeTlsGetValue(XDWORD dwTlsIndex);
XBXKRNL XBOOL KeTlsSetValue(XDWORD dwTlsIndex, XLPVOID lpTlsValue);

#endif
