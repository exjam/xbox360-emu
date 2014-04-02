#include "xex.h"
#include "aes.h"
#include "biginputstream.h"

#include "common/log.h"
#include "common/memory.h"

#include <windows.h>

#include <map>
#include <string>
#include <algorithm>
#include <fstream>

namespace xex
{

bool Loader::load(std::istream &istr, xex::Binary &binary)
{
   BigInputStream in { istr };
   xex::Header &header = binary.header;

   in >> header.magic
      >> header.moduleFlags.flags
      >> header.peDataOffset
      >> header.reserved
      >> header.securityInfoOffset
      >> header.optHeaderCount;

   for (uint32_t i = 0; i < header.optHeaderCount; ++i) {
      uint32_t id, dataOffset, length;
      uint32_t pos, read, start, end;
         
      in
         >> id
         >> dataOffset;

      pos = in.tell();

      /* Get header length */
      length = id & 0xFF;

      if (length == 0 || length == 1) {
         in.skip(-4);
         length = 4;
      } else if (length == 0xFF) {
         in.seek(dataOffset);
         in >> length;
         length -= 4;
      } else {
         in.seek(dataOffset);
         length = length * 4;
      }

      start = in.tell();
                  
      /* Parse header data */
      switch (static_cast<Headers>(id >> 8)) {
      case xex::Headers::ResourceInfo:
         readHeader(in, length, header.resourceInfo);
         break;
      case xex::Headers::BaseFileFormat:
         readHeader(in, length, header.baseFileFormat);
         break;
      case xex::Headers::BaseReference:
         readHeader(in, length, header.baseReference);
         break;
      case xex::Headers::DeltaPatchDescriptor:
         readHeader(in, length, header.deltaPatchDescriptor);
         break;
      case xex::Headers::BoundingPath:
         readHeader(in, length, header.boundingPath);
         break;
      case xex::Headers::DeviceId:
         readHeader(in, length, header.deviceId);
         break;
      case xex::Headers::OriginalBaseAddress:
         readHeader(in, length, header.originalBaseAddress);
         break;
      case xex::Headers::EntryPoint:
         readHeader(in, length, header.entryPoint);
         break;
      case xex::Headers::ImageBaseAddress:
         readHeader(in, length, header.imageBaseAddress);
         break;
      case xex::Headers::ImportLibraries:
         readHeader(in, length, header.importLibraries);
         break;
      case xex::Headers::ChecksumTimestamp:
         readHeader(in, length, header.checksumTimestamp);
         break;
      case xex::Headers::EnabledForCallcap:
         readHeader(in, length, header.enabledForCallcap);
         break;
      case xex::Headers::EnabledForFastcap:
         readHeader(in, length, header.enabledForFastcap);
         break;
      case xex::Headers::OriginalPeName:
         readHeader(in, length, header.originalPeName);
         break;
      case xex::Headers::StaticLibraries:
         readHeader(in, length, header.staticLibraries);
         break;
      case xex::Headers::TlsInfo:
         readHeader(in, length, header.tlsInfo);
         break;
      case xex::Headers::DefaultStackSize:
         readHeader(in, length, header.defaultStackSize);
         break;
      case xex::Headers::DefaultFilesystemCacheSize:
         readHeader(in, length, header.defaultFilesystemCacheSize);
         break;
      case xex::Headers::DefaultHeapSize:
         readHeader(in, length, header.defaultHeapSize);
         break;
      case xex::Headers::PageHeapSizeandFlags:
         readHeader(in, length, header.pageHeapSizeandFlags);
         break;
      case xex::Headers::SystemFlags:
         readHeader(in, length, header.systemFlags);
         break;
      case xex::Headers::ExecutionInfo:
         readHeader(in, length, header.executionInfo);
         break;
      case xex::Headers::ServiceIdList:
         readHeader(in, length, header.serviceIdList);
         break;
      case xex::Headers::TitleWorkspaceSize:
         readHeader(in, length, header.titleWorkspaceSize);
         break;
      case xex::Headers::GameRatings:
         readHeader(in, length, header.gameRatings);
         break;
      case xex::Headers::LanKey:
         readHeader(in, length, header.lanKey);
         break;
      case xex::Headers::Xbox360Logo:
         readHeader(in, length, header.xbox360Logo);
         break;
      case xex::Headers::MultidiscMediaIds:
         readHeader(in, length, header.multidiscMediaIds);
         break;
      case xex::Headers::AlternateTitleIds:
         readHeader(in, length, header.alternateTitleIds);
         break;
      case xex::Headers::AdditionalTitleMemory:
         readHeader(in, length, header.additionalTitleMemory);
         break;
      case xex::Headers::ExportsByName:
         readHeader(in, length, header.exportsByName);
         break;
      default:
         xDebug() << "Unknown header found 0x" << Log::hex(id, 8);
      }

      end = in.tell();
      read = end - start;
      xDebug() << "Header 0x" << Log::hex(id, 8) << " read " << read << " / " << length << " bytes";

      in.seek(pos);
   }

   in.seek(header.securityInfoOffset);
   readHeader(in, 0, header.loaderInfo);

   uint32_t sectionCount;
   in >> sectionCount;

   for (uint32_t i = 0; i < sectionCount; ++i) {
      xex::Section section;

      in >> section._typePageCount
         >> section.digest;

      header.sections.push_back(section);
   }

   /* Decrypt Header Key */
   const static uint8_t xex2_retail_key[16] = {
      0x20, 0xB1, 0x85, 0xA5, 0x9D, 0x28, 0xFD, 0xC3,
      0x40, 0x58, 0x3F, 0xBB, 0x08, 0x96, 0xBF, 0x91
   };

   const static uint8_t xex2_devkit_key[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };

   Rijndael { xex2_retail_key }.decrypt(header.loaderInfo.fileKey, header.sessionKey);

   /* Read Image */
   readImage(in, header);

   /* Load windows PE from memory */
   loadPe(binary, header.imageBaseAddress.address);
   loadImportLibraries(header.importLibraries);

   return true;
}

bool Loader::readImage(BigInputStream &in, xex::Header &header)
{
   switch (header.baseFileFormat.compression.type) {
   case xex::BaseFileFormat::Compression::None:
      return readImageUncompressed(in, header);
   case xex::BaseFileFormat::Compression::Basic:
      return readImageBasicCompression(in, header);
   case xex::BaseFileFormat::Compression::Normal:
      return readImageNormalCompression(in, header);
   default:
      xDebug() << "Unimplemented readImage compression type " << header.baseFileFormat.compression.type;
   }

   return false;
}

bool Loader::readImageUncompressed(BigInputStream &in, xex::Header &header)
{
   return false;
}

bool Loader::readImageNormalCompression(BigInputStream &in, xex::Header &header)
{
   return false;
}

bool Loader::readImageBasicCompression(BigInputStream &in, xex::Header &header)
{
   unsigned uncompressedSize;

   in.seek(header.peDataOffset);

   /* Calculate uncompressed size */
   uncompressedSize = 0;

   for (auto block : header.baseFileFormat.compression.basic.blocks) {
      uncompressedSize += block.dataSize + block.zeroSize;
   }

   /* Allocate virtual memory */
   uint8_t *pExecutable = Memory::allocate(header.imageBaseAddress.address, uncompressedSize);

   if (!pExecutable) {
      xDebug() << "Could not allocate memory at 0x" << Log::hex(header.imageBaseAddress.address, 8) << " with size " << uncompressedSize;
      return false;
   }

   switch(header.baseFileFormat.encryption.type) {
   /*case xex::BaseFileFormat::Encryption::Unencrypted:
      break;*/
   case xex::BaseFileFormat::Encryption::Encrypted:
   {
      Rijndael crypt;
      uint8_t buffer[16];
      uint8_t *pMemory;

      crypt.init(header.sessionKey);
      pMemory = pExecutable;

      for (auto block : header.baseFileFormat.compression.basic.blocks) {
         for (size_t n = 0; n < block.dataSize; n += 16) {
            in >> buffer;
            crypt.decrypt(buffer, pMemory + n);
         }

         pMemory += block.dataSize + block.zeroSize;
      }
      break;
   }
   default:
      xDebug() << "Unimplemented readImageBasicCompression encryption type " << header.baseFileFormat.encryption.type;
   }

   return true;
}

bool Loader::loadPe(xex::Binary &binary, uint32_t exeAddress)
{
   /* Verify DOS Header */
   const PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(exeAddress);

   if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
      xDebug() << "IMAGE_DOS_HEADER.e_magic != IMAGE_DOS_SIGNATURE";
      return false;
   }

   /* Verify NT32 Header */
   const PIMAGE_NT_HEADERS32 ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS32>(exeAddress + dosHeader->e_lfanew);
      
   if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
      xDebug() << "IMAGE_NT_HEADERS32.Signature != IMAGE_NT_SIGNATURE";
      return false;
   }

   /* Verify File Header */
   const PIMAGE_FILE_HEADER fileHeader = &ntHeader->FileHeader;

   if (fileHeader->Machine != IMAGE_FILE_MACHINE_POWERPCBE) {
      xDebug() << "IMAGE_FILE_HEADER.Machine != IMAGE_FILE_MACHINE_POWERPCBE";
      return false;
   }
      
   if (!(fileHeader->Characteristics & IMAGE_FILE_32BIT_MACHINE)) {
      xDebug() << "IMAGE_FILE_HEADER.Characteristics & IMAGE_FILE_32BIT_MACHINE == 0";
      return false;
   }

   if (fileHeader->SizeOfOptionalHeader != IMAGE_SIZEOF_NT_OPTIONAL_HEADER) {
      xDebug() << "IMAGE_FILE_HEADER.SizeOfOptionalHeader != IMAGE_SIZEOF_NT_OPTIONAL_HEADER";
      return false;
   }

   /* Verify Optional Header */
   const PIMAGE_OPTIONAL_HEADER32 optHeader = &ntHeader->OptionalHeader;

   if (optHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      xDebug() << "IMAGE_OPTIONAL_HEADER32.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC";
      return false;
   }

   if (optHeader->Subsystem != IMAGE_SUBSYSTEM_XBOX) {
      xDebug() << "IMAGE_OPTIONAL_HEADER32.Subsystem != IMAGE_SUBSYSTEM_XBOX";
      return false;
   }

   /* Read Section Headers */
   PIMAGE_SECTION_HEADER secHeader = IMAGE_FIRST_SECTION(ntHeader);

   for (WORD i = 0; i < fileHeader->NumberOfSections; ++i, ++secHeader) {
      PeSection section;
      section.name = std::string { reinterpret_cast<const char*>(secHeader->Name), 8 };
      section.rawAddress = secHeader->PointerToRawData;
      section.rawSize = secHeader->SizeOfRawData;
      section.address = exeAddress + secHeader->VirtualAddress;
      section.size = secHeader->Misc.VirtualSize;
      section.flags = secHeader->Characteristics;
      binary.sections.push_back(section);
   }

   return true;
}

void Loader::loadImportLibraries(xex::ImportLibraries &importLibraries)
{
   for (auto &library : importLibraries.libraries) {
      for (auto &record : library.records) {
         auto value = Memory::read<uint32_t>(record.address);
         auto ordinal = value & 0xFFFF;
         auto type = value >> 24;

         switch (type) {
         case xex::ImportLibraries::ImportOrdinal:
         {
            xex::ImportLibrary::Import import;
            import.address = record.address;
            import.ordinal = ordinal;
            import.thunk   = 0;
            import.args    = 0;
            import.handle  = nullptr;
            library.imports.push_back(import);
            break;
         }
         case xex::ImportLibraries::ImportThunk:
         {
            xex::ImportLibrary::Import &import = library.imports.back();
            import.thunk = record.address;
            assert(import.ordinal == ordinal);
            break;
         }
         default:
            xDebug() << "Unknown ImportLibraries record type " << type;
         }
      }
   }
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::ResourceInfo &resourceInfo)
{
   in >> resourceInfo.titleId
      >> resourceInfo.address
      >> resourceInfo.size;
}
   
void Loader::readHeader(BigInputStream &in, unsigned length, xex::BaseFileFormat &baseFileFormat)
{
   in >> baseFileFormat.encryption.type
      >> baseFileFormat.compression.type;

   switch (baseFileFormat.compression.type) {
   case xex::BaseFileFormat::Compression::Basic:
      {
         unsigned blocks = (length - 4) / 8;

         for (unsigned i = 0; i < blocks; ++i) {
            xex::BaseFileFormat::BasicCompression::Block block;

            in >> block.dataSize
               >> block.zeroSize;

            baseFileFormat.compression.basic.blocks.push_back(block);
         }
         break;
      }
   case xex::BaseFileFormat::Compression::Normal:
      in >> baseFileFormat.compression.normal.windowSize
         >> baseFileFormat.compression.normal.blockSize
         >> baseFileFormat.compression.normal.blockHash;
      break;
   case xex::BaseFileFormat::Compression::None:
   case xex::BaseFileFormat::Compression::Delta:
      xDebug() << "Unimplemented BaseFileFormat.compressionType " << baseFileFormat.compression.type;
      break;
   default:
      xDebug() << "Unknown BaseFileFormat.compressionType " << baseFileFormat.compression.type;
   }
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::EntryPoint &entryPoint)
{
   in >> entryPoint.address;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::ImageBaseAddress &imageBaseAddress)
{
   in >> imageBaseAddress.address;
}

void Loader::readStringTable(BigInputStream &in, unsigned length, std::vector<std::string> &stringTable)
{
   std::string curString;

   for (unsigned i = 0; i < length; ++i) {
      uint8_t c;
      in >> c;

      if (c) {
         curString.push_back(c);
      } else if (curString.length() || (i + 1) == length) {
         stringTable.push_back(curString);
         curString.clear();
      }
   }
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::ImportLibraries &importLibraries)
{
   std::vector<std::string> stringTable;
   uint32_t stringTableSize;
   uint32_t libraryCount;

   in >> stringTableSize
      >> libraryCount;

   readStringTable(in, stringTableSize, stringTable);

   for (unsigned i = 0; i < libraryCount; ++i) {
      xex::ImportLibrary library;
      uint16_t nameIndex;
      uint16_t importCount;

      in >> library.unknown
         >> library.digest
         >> library.importId
         >> library.curVersion.value
         >> library.minVersion.value
         >> nameIndex
         >> importCount;

      library.name = stringTable.at(nameIndex);
         
      for (uint16_t j = 0; j < importCount; ++j) {
         xex::ImportLibrary::Record record;
         in >> record.address;
         library.records.push_back(record);
      }

      importLibraries.libraries.push_back(library);
   }
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::ChecksumTimestamp &checksumTimestamp)
{
   in >> checksumTimestamp.checksum
      >> checksumTimestamp.timestamp;
}
   
void Loader::readHeader(BigInputStream &in, unsigned length, xex::OriginalPeName &originalPeName)
{
   in >> NullPadString(originalPeName.name);
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::StaticLibraries &staticLibraries)
{
   unsigned count = length / 16;

   for (unsigned i = 0; i < count; ++i) {
      xex::StaticLibrary library;

      in >> FixedWidthString(library.name, 8)
         >> library.version.major
         >> library.version.minor
         >> library.version.build
         >> library.version._qfeApproved;

      staticLibraries.libraries.push_back(library);
   }
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::TlsInfo &tlsInfo)
{
   in >> tlsInfo.slotCount
      >> tlsInfo.rawDataAddress
      >> tlsInfo.dataSize
      >> tlsInfo.rawDataSize;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::DefaultStackSize &defaultStackSize)
{
   in >> defaultStackSize.size;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::DefaultHeapSize &defaultHeapSize)
{
   in >> defaultHeapSize.size;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::SystemFlags &systemFlags)
{
   in >> systemFlags.flags;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::ExecutionInfo &executionInfo)
{
   in >> executionInfo.mediaId
      >> executionInfo.curVersion.value
      >> executionInfo.baseVersion.value
      >> executionInfo.titleId
      >> executionInfo.platform
      >> executionInfo.executableTable
      >> executionInfo.discNumber
      >> executionInfo.discCount
      >> executionInfo.saveGameId;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::GameRatings &gameRatings)
{
   in >> gameRatings.esrb
      >> gameRatings.pegi
      >> gameRatings.pegifi
      >> gameRatings.pegipt
      >> gameRatings.bbfc
      >> gameRatings.cero
      >> gameRatings.usk
      >> gameRatings.oflcau
      >> gameRatings.oflcnz
      >> gameRatings.kmrb
      >> gameRatings.brazil
      >> gameRatings.fpb;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::LanKey &lanKey)
{
   in >> lanKey.key;
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::AlternateTitleIds &alternateTitleIds)
{
   unsigned count = length / 4;

   for (unsigned i = 0; i < count; ++i) {
      uint32_t id;
      in >> id;
      alternateTitleIds.ids.push_back(id);
   }
}

void Loader::readHeader(BigInputStream &in, unsigned length, xex::LoaderInfo &loaderInfo)
{
   in >> loaderInfo.headerSize
      >> loaderInfo.imageSize
      >> loaderInfo.rsaSignature
      >> loaderInfo._unknown1
      >> loaderInfo.imageFlags._flags
      >> loaderInfo.loadAddress
      >> loaderInfo.sectionDigest
      >> loaderInfo.importTableCount
      >> loaderInfo.importTableDigest
      >> loaderInfo.mediaId
      >> loaderInfo.fileKey
      >> loaderInfo.exportTable
      >> loaderInfo.headerDigest
      >> loaderInfo.gameRegions
      >> loaderInfo.mediaFlags._flags;
}

} //namespace xex
