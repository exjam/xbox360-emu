#include "xex.h"
#include "xboxkrnl.h"

#include "xex/xex.h"
#include "common/memory.h"

#include <Windows.h>

using XLPXEXBINARY = XLP<xex::Binary>;

XLP<XLDR_DATA_TABLE_ENTRY> XexExecutableModuleHandle;

XBXKRNL XDWORD
XexGetModuleHandle(XLPCHAR lpModuleName,
                   XLPDWORD lpHandle)
{
   HMODULE handle = GetModuleHandleA(lpModuleName);

   if (handle) {
      Memory::write(lpHandle, reinterpret_cast<XDWORD>(handle));
      return 0;
   } else {
      return -1;
   }
}

XBXKRNL XBOOL
XexCheckExecutablePrivilege(XDWORD priviledge)
{
   auto sysFlags = g_kernel->getXexBinary()->header.systemFlags;
   return ((sysFlags.flags >> priviledge) & 1);
}

XBXKRNL XPVOID
RtlImageXexHeaderField(XLPVOID XexHeaderBase,
                       XDWORD Key)
{
   XLPXEXBINARY binary;
   XDWORD length = Key & 0xFF;
   void *result = nullptr;

   binary = XexHeaderBase;

   switch (static_cast<xex::Headers>(Key >> 8)) {
   case xex::Headers::ResourceInfo:
      result = &binary->header.resourceInfo;
      break;
   case xex::Headers::BaseFileFormat:
      result = &binary->header.baseFileFormat;
      break;
   case xex::Headers::BaseReference:
      result = &binary->header.baseReference;
      break;
   case xex::Headers::DeltaPatchDescriptor:
      result = &binary->header.deltaPatchDescriptor;
      break;
   case xex::Headers::BoundingPath:
      result = &binary->header.boundingPath;
      break;
   case xex::Headers::DeviceId:
      result = &binary->header.deviceId;
      break;
   case xex::Headers::OriginalBaseAddress:
      result = &binary->header.originalBaseAddress;
      break;
   case xex::Headers::EntryPoint:
      result = &binary->header.entryPoint;
      break;
   case xex::Headers::ImageBaseAddress:
      result = &binary->header.imageBaseAddress;
      break;
   case xex::Headers::ImportLibraries:
      result = &binary->header.importLibraries;
      break;
   case xex::Headers::ChecksumTimestamp:
      result = &binary->header.checksumTimestamp;
      break;
   case xex::Headers::EnabledForCallcap:
      result = &binary->header.enabledForCallcap;
      break;
   case xex::Headers::EnabledForFastcap:
      result = &binary->header.enabledForFastcap;
      break;
   case xex::Headers::OriginalPeName:
      result = &binary->header.originalPeName;
      break;
   case xex::Headers::StaticLibraries:
      result = &binary->header.staticLibraries;
      break;
   case xex::Headers::TlsInfo:
      result = &binary->header.tlsInfo;
      break;
   case xex::Headers::DefaultStackSize:
      result = &binary->header.defaultStackSize;
      break;
   case xex::Headers::DefaultFilesystemCacheSize:
      result = &binary->header.defaultFilesystemCacheSize;
      break;
   case xex::Headers::DefaultHeapSize:
      result = &binary->header.defaultHeapSize;
      break;
   case xex::Headers::PageHeapSizeandFlags:
      result = &binary->header.pageHeapSizeandFlags;
      break;
   case xex::Headers::SystemFlags:
      result = &binary->header.systemFlags;
      break;
   case xex::Headers::ExecutionInfo:
      result = &binary->header.executionInfo;
      break;
   case xex::Headers::ServiceIdList:
      result = &binary->header.serviceIdList;
      break;
   case xex::Headers::TitleWorkspaceSize:
      result = &binary->header.titleWorkspaceSize;
      break;
   case xex::Headers::GameRatings:
      result = &binary->header.gameRatings;
      break;
   case xex::Headers::LanKey:
      result = &binary->header.lanKey;
      break;
   case xex::Headers::Xbox360Logo:
      result = &binary->header.xbox360Logo;
      break;
   case xex::Headers::MultidiscMediaIds:
      result = &binary->header.multidiscMediaIds;
      break;
   case xex::Headers::AlternateTitleIds:
      result = &binary->header.alternateTitleIds;
      break;
   case xex::Headers::AdditionalTitleMemory:
      result = &binary->header.additionalTitleMemory;
      break;
   case xex::Headers::ExportsByName:
      result = &binary->header.exportsByName;
      break;
   default:
      xDebug() << "Unknown header found 0x" << Log::hex(Key, 8);
   }

   return reinterpret_cast<XPVOID>(reinterpret_cast<uint32_t>(result) + sizeof(binary->header.additionalTitleMemory._read));
}

void xexInit()
{
   xex::Binary *binary = g_kernel->getXexBinary();
   auto module = new XLDR_DATA_TABLE_ENTRY();

   memset(module, 0, sizeof(XLDR_DATA_TABLE_ENTRY));
   module->XexHeaderBase = bits::swap(binary);

   XexExecutableModuleHandle = bits::swap(module);
}
