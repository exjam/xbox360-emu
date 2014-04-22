#ifndef XBXKRNL_XEX_H
#define XBXKRNL_XEX_H

#include "kernel/kernel.h"

void xexInit();

#pragma pack(push, 1)
struct KAnsiString // ANSI_STRING
{
   uint16_t Length;
   uint16_t MaximumLength;
   ptr32<int8_t> Buffer;
};

struct KUnicodeString // UNICODE_STRING
{
   uint16_t Length;
   uint16_t MaximumLength;
   ptr32<int16_t> Buffer;
};

struct KLdrDataTableEntry
{
   LinkedListEntry InLoadOrderLinks;
   LinkedListEntry InClosureOrderLinks;
   LinkedListEntry InInitializationOrderLinks;
   ptr32<void>     NtHeadersBase;
   ptr32<void>     ImageBase;
   uint32_t        SizeOfNtImage;
   KUnicodeString  FullDllName;
   KUnicodeString  BaseDllName;
   uint32_t        Flags;
   uint32_t        SizeOfFullImage;
   ptr32<void>     EntryPoint;
   uint16_t        LoadCount;
   uint16_t        ModuleIndex;
   ptr32<void>     DllBaseOriginal;
   uint32_t        CheckSum;
   uint32_t        ModuleLoadFlags;
   uint32_t        TimeDateStamp;
   ptr32<void>     LoadedImports;
   ptr32<void>     XexHeaderBase;

   struct
   {
      ptr32<KLdrDataTableEntry> ClosureRoot;
      ptr32<KLdrDataTableEntry> TraversalParent;
   } asEntry;
};

#pragma pack(pop)

XBXKRNL ptr32<KLdrDataTableEntry> XexExecutableModuleHandle;

XBXKRNL uint32_t
XexGetModuleHandle(ptr32<char> lpModuleName,
                   ptr32<uint32_t> lpHandle);
XBXKRNL uint64_t
XexCheckExecutablePrivilege(uint32_t priviledge);

XBXKRNL void*
RtlImageXexHeaderField(ptr32<void> XexHeaderBase,
                       uint32_t Key);

#endif // ifndef XBXKRNL_XEX_H
