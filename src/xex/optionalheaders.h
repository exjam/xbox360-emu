#ifndef XEX_OPTIONAL_HEADERS_H
#define XEX_OPTIONAL_HEADERS_H

#include <stdint.h>
#include <string>
#include <vector>

namespace xex
{

enum class Headers
{
   ResourceInfo               =    0x2,
   BaseFileFormat             =    0x3,
   BaseReference              =    0x4,
   DeltaPatchDescriptor       =    0x5,
   BoundingPath               =   0x80,
   DeviceId                   =   0x81,
   OriginalBaseAddress        =  0x100,
   EntryPoint                 =  0x101,
   ImageBaseAddress           =  0x102,
   ImportLibraries            =  0x103,
   ChecksumTimestamp          =  0x180,
   EnabledForCallcap          =  0x181,
   EnabledForFastcap          =  0x182,
   OriginalPeName             =  0x183,
   StaticLibraries            =  0x200,
   TlsInfo                    =  0x201,
   DefaultStackSize           =  0x202,
   DefaultFilesystemCacheSize =  0x203,
   DefaultHeapSize            =  0x204,
   PageHeapSizeandFlags       =  0x280,
   SystemFlags                =  0x300,
   ExecutionInfo              =  0x400,
   ServiceIdList              =  0x401,
   TitleWorkspaceSize         =  0x402,
   GameRatings                =  0x403,
   LanKey                     =  0x404,
   Xbox360Logo                =  0x405,
   MultidiscMediaIds          =  0x406,
   AlternateTitleIds          =  0x407,
   AdditionalTitleMemory      =  0x408,
   ExportsByName              = 0xE104,
};

#pragma pack(push, 1)

struct OptionalRawHeader
{
   uint32_t key;
   std::vector<uint8_t> data;
};

struct OptionalHeader {
   OptionalHeader() : _read(false) {}
   bool _read;
};

union Version
{
   uint32_t value;

   struct {
      uint32_t qfe : 8;
      uint32_t build : 16;
      uint32_t minor : 4;
      uint32_t major : 4;
   };
};

union TitleId
{
   uint32_t value;

   struct {
      uint8_t letters[2];
      uint16_t numbers;
   };
};

struct ResourceInfo : public OptionalHeader
{
   static const Headers id = Headers::ResourceInfo;

   uint8_t titleId[8];
   uint32_t address;
   uint32_t size;
};

struct BaseFileFormat : public OptionalHeader
{
   static const Headers id = Headers::BaseFileFormat;

   struct BasicCompression {
      struct Block {
         uint32_t dataSize;
         uint32_t zeroSize;
      };

      std::vector<Block> blocks;
   };

   struct NormalCompression {
      uint32_t windowSize;
      uint32_t blockSize;
      uint8_t blockHash[20];
   };

   struct Compression {
      enum {
         None   = 0x00,
         Basic  = 0x01,
         Normal = 0x02,
         Delta  = 0x03
      };

      uint16_t type;
      BasicCompression basic;
      NormalCompression normal;
   };

   struct Encryption {
      enum {
         Unencrypted = 0x00,
         Encrypted   = 0x01
      };

      uint16_t type;
   };

   Encryption encryption;
   Compression compression;
};

struct BaseReference : public OptionalHeader
{
   static const Headers id = Headers::BaseReference;
};

struct DeltaPatchDescriptor : public OptionalHeader
{
   static const Headers id = Headers::DeltaPatchDescriptor;
};

struct BoundingPath : public OptionalHeader
{
   static const Headers id = Headers::BoundingPath;
};

struct DeviceId : public OptionalHeader
{
   static const Headers id = Headers::DeviceId;
};

struct OriginalBaseAddress : public OptionalHeader
{
   static const Headers id = Headers::OriginalBaseAddress;
};

struct EntryPoint : public OptionalHeader
{
   static const Headers id = Headers::EntryPoint;

   uint32_t address;
};

struct ImageBaseAddress : public OptionalHeader
{
   static const Headers id = Headers::ImageBaseAddress;

   uint32_t address;
};

struct ImportLibrary : public OptionalHeader
{
   struct Record {
      uint32_t address;
   };

   struct Import {
      uint32_t address;
      uint32_t ordinal;
      uint32_t thunk;
      uint32_t args;
      std::string name;
   };

   uint32_t unknown;
   uint8_t digest[20];
   uint32_t importId;
   Version curVersion;
   Version minVersion;
   std::string name;
   std::vector<Record> records;
   std::vector<Import> imports;
};

struct ImportLibraries : public OptionalHeader
{
   enum
   {
      ImportOrdinal = 0,
      ImportThunk = 1,
   };

   static const Headers id = Headers::ImportLibraries;

   std::vector<ImportLibrary> libraries;
};

struct ChecksumTimestamp : public OptionalHeader
{
   static const Headers id = Headers::ChecksumTimestamp;

   uint32_t checksum;
   uint32_t timestamp;
};

struct EnabledForCallcap : public OptionalHeader
{
   static const Headers id = Headers::EnabledForCallcap;
};

struct EnabledForFastcap : public OptionalHeader
{
   static const Headers id = Headers::EnabledForFastcap;
};

struct OriginalPeName : public OptionalHeader
{
   static const Headers id = Headers::OriginalPeName;

   std::string name;
};

struct StaticLibrary : public OptionalHeader
{
   std::string name;

   struct {
      uint16_t major;
      uint16_t minor;
      uint16_t build;

      union {
         uint16_t _qfeApproved;

         struct {
            uint16_t qfe : 15;
            uint16_t approved : 1;
         };
      };
   } version;
};

struct StaticLibraries : public OptionalHeader
{
   static const Headers id = Headers::StaticLibraries;

   std::vector<StaticLibrary> libraries;
};

struct TlsInfo : public OptionalHeader
{
   static const Headers id = Headers::TlsInfo;

   uint32_t slotCount;
   uint32_t rawDataAddress;
   uint32_t dataSize;
   uint32_t rawDataSize;
};

struct DefaultStackSize : public OptionalHeader
{
   static const Headers id = Headers::DefaultStackSize;

   uint32_t size;
};

struct DefaultFilesystemCacheSize : public OptionalHeader
{
   static const Headers id = Headers::DefaultFilesystemCacheSize;
};

struct DefaultHeapSize : public OptionalHeader
{
   static const Headers id = Headers::DefaultHeapSize;

   uint32_t size;
};

struct PageHeapSizeandFlags : public OptionalHeader
{
   static const Headers id = Headers::PageHeapSizeandFlags;
};

struct SystemFlags : public OptionalHeader
{
   static const Headers id = Headers::SystemFlags;

   union {
      uint32_t flags;

      struct {
         uint32_t noForcedReboot : 1;
         uint32_t foregroundTasks : 1;
         uint32_t noOddMapping : 1;
         uint32_t handlesMceInputs : 1;
         uint32_t restrictedHudFeatures : 1;
         uint32_t handlesGamepadDisconnect : 1;
         uint32_t hasSecureSockets : 1;
         uint32_t xbox1Interop : 1;
         uint32_t dashContext : 1;
         uint32_t usesGameVoiceChannel : 1;
         uint32_t pal50Incompatible : 1;
         uint32_t insecureUtilityDriveSupport : 1;
         uint32_t xamHooks : 1;
         uint32_t accessesPii : 1;
         uint32_t crossPlatformSystemLink : 1;
         uint32_t multidiscSwap : 1;
         uint32_t supportsInsecureMultidiscMedia : 1;
         uint32_t antipiracy25Media : 1;
         uint32_t noConfirmExit : 1;
         uint32_t allowBackgroundDownloading : 1;
         uint32_t createPersistentRamDrive : 1;
         uint32_t inheritPersistentRamDrive : 1;
         uint32_t allowHudVibration : 1;
         uint32_t allowBothUtilityPartitionAccess : 1;
         uint32_t handleIptvInput : 1;
         uint32_t preferBigButtonInput : 1;
         uint32_t allowExtendedSystemReservation : 1;
         uint32_t multidiscCrossTitle : 1;
         uint32_t titleInstallIncompatible : 1;
         uint32_t allowAvatarGetMetadataByXuid : 1;
         uint32_t allowControllerSwapping : 1;
         uint32_t dashExtensibilityModule : 1;
      };
   };
};

struct ExecutionInfo : public OptionalHeader
{
   static const Headers id = Headers::ExecutionInfo;

   uint32_t mediaId;
   Version curVersion;
   Version baseVersion;
   uint32_t titleId;
   uint8_t platform;
   uint8_t executableTable;
   uint8_t discNumber;
   uint8_t discCount;
   uint32_t saveGameId;
};

struct ServiceIdList : public OptionalHeader
{
   static const Headers id = Headers::ServiceIdList;
};

struct TitleWorkspaceSize : public OptionalHeader
{
   static const Headers id = Headers::TitleWorkspaceSize;
};

struct GameRatings : public OptionalHeader
{
   static const Headers id = Headers::GameRatings;

   uint8_t esrb;
   uint8_t pegi;
   uint8_t pegifi;
   uint8_t pegipt;
   uint8_t bbfc;
   uint8_t cero;
   uint8_t usk;
   uint8_t oflcau;
   uint8_t oflcnz;
   uint8_t kmrb;
   uint8_t brazil;
   uint8_t fpb;
};

struct LanKey : public OptionalHeader
{
   static const Headers id = Headers::LanKey;

   uint8_t key[16];
};

struct Xbox360Logo : public OptionalHeader
{
   static const Headers id = Headers::Xbox360Logo;
};

struct MultidiscMediaIds : public OptionalHeader
{
   static const Headers id = Headers::MultidiscMediaIds;
};

struct AlternateTitleIds : public OptionalHeader
{
   static const Headers id = Headers::AlternateTitleIds;

   std::vector<uint32_t> ids;
};

struct AdditionalTitleMemory : public OptionalHeader {
   static const Headers id = Headers::AdditionalTitleMemory;
};

struct ExportsByName : public OptionalHeader {
   static const Headers id = Headers::ExportsByName;
};

#pragma pack(pop)

} // namespace xex

#endif // XEX_OPTIONAL_HEADERS_H
