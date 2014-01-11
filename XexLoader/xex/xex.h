#ifndef XEXLOADER_XEX_H
#define XEXLOADER_XEX_H

#include <stdint.h>
#include <istream>
#include <string>
#include <vector>

#include <util/log.h>
#include <util/biginputstream.h>

#include "optionalheaders.h"

#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER   224
#define IMAGE_FILE_MACHINE_POWERPCBE      0x01F2

namespace xex
{
   
#pragma pack(push, 1)

union ImageFlags
{
   uint32_t _flags;

   struct {
      uint32_t _unknown_00000001 : 1;
      uint32_t manufacturingUtility : 1;
      uint32_t manufacturingSupportTools : 1;
      uint32_t xgd2MediaOnly : 1;
      uint32_t _unknown_00000010 : 1;
      uint32_t _unknown_00000020 : 1;
      uint32_t _unknown_00000040 : 1;
      uint32_t _unknown_00000080 : 1;
      uint32_t cardeaKey : 1;
      uint32_t xeikaKey : 1;
      uint32_t userModeTitle : 1;
      uint32_t userModeSystem : 1;
      uint32_t orange0 : 1;
      uint32_t orange1 : 1;
      uint32_t orange2 : 1;
      uint32_t _unknown_00008000 : 1;
      uint32_t iptvSignupApplication : 1;
      uint32_t iptvTitleApplication : 1;
      uint32_t _unknown_00040000 : 1;
      uint32_t _unknown_00080000 : 1;
      uint32_t _unknown_00100000 : 1;
      uint32_t _unknown_00200000 : 1;
      uint32_t _unknown_00400000 : 1;
      uint32_t _unknown_00800000 : 1;
      uint32_t _unknown_01000000 : 1;
      uint32_t _unknown_02000000 : 1;
      uint32_t keyvaultPrivilegesRequired : 1;
      uint32_t onlineActivationRequired : 1;
      uint32_t pageSize4KB : 1;
      uint32_t regionFree : 1;
      uint32_t revocationCheckOptional : 1;
      uint32_t revocationCheckRequired : 1;
   };
};

union MediaFlags
{
   uint32_t _flags;

   struct {
      uint32_t hardDisk : 1;
      uint32_t dvdX2 : 1;
      uint32_t dvdCd : 1;
      uint32_t dvd5 : 1;
      uint32_t dvd9 : 1;
      uint32_t systemFlash : 1;
      uint32_t _unknown_00000040 : 1;
      uint32_t memoryUnit : 1;
      uint32_t usbMassStorageDevice : 1;
      uint32_t network : 1;
      uint32_t directFromMemory : 1;
      uint32_t ramDrive : 1;
      uint32_t svod : 1;
      uint32_t _unknown_00002000 : 1;
      uint32_t _unknown_00004000 : 1;
      uint32_t _unknown_00008000 : 1;
      uint32_t _unknown_00010000 : 1;
      uint32_t _unknown_00020000 : 1;
      uint32_t _unknown_00040000 : 1;
      uint32_t _unknown_00080000 : 1;
      uint32_t _unknown_00100000 : 1;
      uint32_t _unknown_00200000 : 1;
      uint32_t _unknown_00400000 : 1;
      uint32_t _unknown_00800000 : 1;
      uint32_t insecurePackage : 1;
      uint32_t saveGamePackage : 1;
      uint32_t locallySignedPackage : 1;
      uint32_t liveSignedPackage : 1;
      uint32_t xboxPackage : 1;
      uint32_t _unknown_20000000 : 1;
      uint32_t _unknown_40000000 : 1;
      uint32_t _unknown_80000000 : 1;
   };
};

struct LoaderInfo
{
   uint32_t headerSize;
   uint32_t imageSize;
   uint8_t rsaSignature[256];
   uint32_t _unknown1;
   ImageFlags imageFlags;
   uint32_t loadAddress;
   uint8_t sectionDigest[20];
   uint32_t importTableCount;
   uint8_t importTableDigest[20];
   uint8_t mediaId[16];
   uint8_t fileKey[16];
   uint32_t exportTable;
   uint8_t headerDigest[20];
   uint32_t gameRegions;
   MediaFlags mediaFlags;
};

struct Section
{
   enum Type {
      Code         = 0x01,
      Data         = 0x02,
      ReadOnlyData = 0x03
   };

   union {
      uint32_t _typePageCount;

      struct {
         uint32_t type : 4;
         uint32_t pageCount : 28;
      };
   };

   uint8_t digest[20];
};

struct ModuleFlags 
{
   union {
      uint32_t flags;

      struct {
         uint32_t title : 1;
         uint32_t exportsToTitle : 1;
         uint32_t systemDebugger : 1;
         uint32_t dllModule : 1;
         uint32_t modulePatch : 1;
         uint32_t patchFull : 1;
         uint32_t patchDelta : 1;
         uint32_t userMode : 1;
         uint32_t unused : 24;
      };
   };
};

struct Header
{
   /* Actual Header */
   uint8_t magic[4];
   ModuleFlags moduleFlags;
   uint32_t peDataOffset;
   uint32_t reserved;
   uint32_t securityInfoOffset;
   uint32_t optHeaderCount;

   /* Security Info */ 
   xex::LoaderInfo loaderInfo;
   std::vector<xex::Section> sections;
   uint8_t sessionKey[16];

   /* Optional Headers */
   ResourceInfo resourceInfo;
   BaseFileFormat baseFileFormat;
   BaseReference baseReference;
   DeltaPatchDescriptor deltaPatchDescriptor;
   BoundingPath boundingPath;
   DeviceId deviceId;
   OriginalBaseAddress originalBaseAddress;
   EntryPoint entryPoint;
   ImageBaseAddress imageBaseAddress;
   ImportLibraries importLibraries;
   ChecksumTimestamp checksumTimestamp;
   EnabledForCallcap enabledForCallcap;
   EnabledForFastcap enabledForFastcap;
   OriginalPeName originalPeName;
   StaticLibraries staticLibraries;
   TlsInfo tlsInfo;
   DefaultStackSize defaultStackSize;
   DefaultFilesystemCacheSize defaultFilesystemCacheSize;
   DefaultHeapSize defaultHeapSize;
   PageHeapSizeandFlags pageHeapSizeandFlags;
   SystemFlags systemFlags;
   ExecutionInfo executionInfo;
   ServiceIdList serviceIdList;
   TitleWorkspaceSize titleWorkspaceSize;
   GameRatings gameRatings;
   LanKey lanKey;
   Xbox360Logo xbox360Logo;
   MultidiscMediaIds multidiscMediaIds;
   AlternateTitleIds alternateTitleIds;
   AdditionalTitleMemory additionalTitleMemory;
   ExportsByName exportsByName;
};

struct PeSection
{
   std::string name;

   uint32_t rawAddress;
   uint32_t rawSize;
   uint32_t address;
   uint32_t size;
   uint32_t flags;
};

#pragma pack(pop)

struct Binary
{
   Header header;
   std::vector<xex::PeSection> sections;
};

class Loader
{
public:
   bool load(std::istream &istr, xex::Binary &binary);

protected:
   bool loadPe(xex::Binary &binary, uint64_t exeAddress);

   void loadImportLibraries(xex::ImportLibraries &importLibraries);

   bool readImage(BigInputStream &in, xex::Header &header);
   bool readImageUncompressed(BigInputStream &in, xex::Header &header);
   bool readImageNormalCompression(BigInputStream &in, xex::Header &header);
   bool readImageBasicCompression(BigInputStream &in, xex::Header &header);

   void readHeader(BigInputStream &in, unsigned length, xex::ResourceInfo &out);
   void readHeader(BigInputStream &in, unsigned length, xex::BaseFileFormat &out);
   void readHeader(BigInputStream &in, unsigned length, xex::EntryPoint &out);
   void readHeader(BigInputStream &in, unsigned length, xex::ImageBaseAddress &out);
   void readHeader(BigInputStream &in, unsigned length, xex::ImportLibraries &out);
   void readHeader(BigInputStream &in, unsigned length, xex::ChecksumTimestamp &out);
   void readHeader(BigInputStream &in, unsigned length, xex::OriginalPeName &out);
   void readHeader(BigInputStream &in, unsigned length, xex::StaticLibraries &out);
   void readHeader(BigInputStream &in, unsigned length, xex::TlsInfo &out);
   void readHeader(BigInputStream &in, unsigned length, xex::DefaultStackSize &out);
   void readHeader(BigInputStream &in, unsigned length, xex::DefaultHeapSize &out);
   void readHeader(BigInputStream &in, unsigned length, xex::SystemFlags &out);
   void readHeader(BigInputStream &in, unsigned length, xex::ExecutionInfo &out);
   void readHeader(BigInputStream &in, unsigned length, xex::GameRatings &out);
   void readHeader(BigInputStream &in, unsigned length, xex::LanKey &out);
   void readHeader(BigInputStream &in, unsigned length, xex::AlternateTitleIds &out);
   void readHeader(BigInputStream &in, unsigned length, xex::LoaderInfo &out);
   void readStringTable(BigInputStream &in,
                        unsigned length,
                        std::vector<std::string> &out);

   template<typename T>
   void readHeader(BigInputStream &bes, unsigned length, T &header)
   {
      xDebug()
         << "Unimplemented header found 0x"
         << Log::hex(static_cast<unsigned>(T::id))
         << " length " << length;

      std::cout << std::endl;
      
      for (unsigned i = 0; i < length; ++i) {
         uint8_t c;
         bes >> c;
         xDebugInline() << Log::hex(c, 2);
      }
   }

private:
};

} // namespace xex

#endif // XEX_H
