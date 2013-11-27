#ifndef XEX_H
#define XEX_H

#include <kernel.h>

void xexInit();

#pragma pack(push, 1)

using XLPLIST_ENTRY = XLP<struct XLIST_ENTRY>;

struct XLIST_ENTRY
{
   XLPLIST_ENTRY Flink;
   XLPLIST_ENTRY Blink;
};

struct XANSI_STRING
{
   XWORD Length;
   XWORD MaximumLength;
   XLPCHAR Buffer;
};

struct XUNICODE_STRING
{
   XWORD Length;
   XWORD MaximumLength;
   XLPWORD Buffer;
};

using XLPLDR_DATA_TABLE_ENTRY = XLP<struct XLDR_DATA_TABLE_ENTRY>;

struct XLDR_DATA_TABLE_ENTRY
{
   XLIST_ENTRY     InLoadOrderLinks;
   XLIST_ENTRY     InClosureOrderLinks;
   XLIST_ENTRY     InInitializationOrderLinks;
   XLPVOID         NtHeadersBase;
   XLPVOID         ImageBase;
   XDWORD          SizeOfNtImage;
   XUNICODE_STRING FullDllName;
   XUNICODE_STRING BaseDllName;
   XDWORD          Flags;
   XDWORD          SizeOfFullImage;
   XLPVOID         EntryPoint;
   XWORD           LoadCount;
   XWORD           ModuleIndex;
   XLPVOID         DllBaseOriginal;
   XDWORD          CheckSum;
   XDWORD          ModuleLoadFlags;
   XDWORD          TimeDateStamp;
   XLPVOID         LoadedImports;
   XLPVOID         XexHeaderBase;

   struct
   {
      XLPLDR_DATA_TABLE_ENTRY ClosureRoot;
      XLPLDR_DATA_TABLE_ENTRY TraversalParent;
   } asEntry;
};

#pragma pack(pop)

XBXKRNL XLPLDR_DATA_TABLE_ENTRY XexExecutableModuleHandle;

XBXKRNL XDWORD
XexGetModuleHandle(XLPCHAR lpModuleName,
                   XLPDWORD lpHandle);
XBXKRNL XBOOL
XexCheckExecutablePrivilege(XDWORD priviledge);

XBXKRNL XPVOID
RtlImageXexHeaderField(XLPVOID XexHeaderBase,
                       XDWORD Key);

#endif
