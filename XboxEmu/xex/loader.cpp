#include "xex/xex.h"
#include "xex/aes.h"

#include "util/log.h"
#include "util/be/memory.h"
#include "util/be/inputstream.h"

#include <windows.h>

template<int N>
std::string stringFromFixedBuffer(uint8_t (&src)[N]) 
{
   char buf[N + 1];
   memcpy(buf, src, N);
   buf[N] = 0;
   return std::string(buf);
}

namespace xex
{

bool Loader::load(std::istream &istr)
{
   be::InputStream bes(istr);
   xex::Header header;

   bes
      >> header.magic
      >> header.moduleFlags.flags
      >> header.peDataOffset
      >> header.reserved
      >> header.securityInfoOffset
      >> header.optHeaderCount;

   for (uint32_t i = 0; i < header.optHeaderCount; ++i) {
      uint32_t id, dataOffset, length;
      uint32_t pos, read, start, end;
         
      bes
         >> id
         >> dataOffset;

      pos = bes.tell();

      /* Get header length */
      length = id & 0xFF;

      if (length == 0 || length == 1) {
         bes.skip(-4);
         length = 4;
      } else if (length == 0xFF) {
         bes.seek(dataOffset);
         bes >> length;
         length -= 4;
      } else {
         bes.seek(dataOffset);
         length = length * 4;
      }

      start = bes.tell();
                  
      /* Parse header data */
      switch (id >> 8) {
      case xex::Headers::ResourceInfo:
         readHeader(bes, length, header.resourceInfo);
         break;
      case xex::Headers::BaseFileFormat:
         readHeader(bes, length, header.baseFileFormat);
         break;
      case xex::Headers::BaseReference:
         readHeader(bes, length, header.baseReference);
         break;
      case xex::Headers::DeltaPatchDescriptor:
         readHeader(bes, length, header.deltaPatchDescriptor);
         break;
      case xex::Headers::BoundingPath:
         readHeader(bes, length, header.boundingPath);
         break;
      case xex::Headers::DeviceId:
         readHeader(bes, length, header.deviceId);
         break;
      case xex::Headers::OriginalBaseAddress:
         readHeader(bes, length, header.originalBaseAddress);
         break;
      case xex::Headers::EntryPoint:
         readHeader(bes, length, header.entryPoint);
         break;
      case xex::Headers::ImageBaseAddress:
         readHeader(bes, length, header.imageBaseAddress);
         break;
      case xex::Headers::ImportLibraries:
         readHeader(bes, length, header.importLibraries);
         break;
      case xex::Headers::ChecksumTimestamp:
         readHeader(bes, length, header.checksumTimestamp);
         break;
      case xex::Headers::EnabledForCallcap:
         readHeader(bes, length, header.enabledForCallcap);
         break;
      case xex::Headers::EnabledForFastcap:
         readHeader(bes, length, header.enabledForFastcap);
         break;
      case xex::Headers::OriginalPeName:
         readHeader(bes, length, header.originalPeName);
         break;
      case xex::Headers::StaticLibraries:
         readHeader(bes, length, header.staticLibraries);
         break;
      case xex::Headers::TlsInfo:
         readHeader(bes, length, header.tlsInfo);
         break;
      case xex::Headers::DefaultStackSize:
         readHeader(bes, length, header.defaultStackSize);
         break;
      case xex::Headers::DefaultFilesystemCacheSize:
         readHeader(bes, length, header.defaultFilesystemCacheSize);
         break;
      case xex::Headers::DefaultHeapSize:
         readHeader(bes, length, header.defaultHeapSize);
         break;
      case xex::Headers::PageHeapSizeandFlags:
         readHeader(bes, length, header.pageHeapSizeandFlags);
         break;
      case xex::Headers::SystemFlags:
         readHeader(bes, length, header.systemFlags);
         break;
      case xex::Headers::ExecutionInfo:
         readHeader(bes, length, header.executionInfo);
         break;
      case xex::Headers::ServiceIdList:
         readHeader(bes, length, header.serviceIdList);
         break;
      case xex::Headers::TitleWorkspaceSize:
         readHeader(bes, length, header.titleWorkspaceSize);
         break;
      case xex::Headers::GameRatings:
         readHeader(bes, length, header.gameRatings);
         break;
      case xex::Headers::LanKey:
         readHeader(bes, length, header.lanKey);
         break;
      case xex::Headers::Xbox360Logo:
         readHeader(bes, length, header.xbox360Logo);
         break;
      case xex::Headers::MultidiscMediaIds:
         readHeader(bes, length, header.multidiscMediaIds);
         break;
      case xex::Headers::AlternateTitleIds:
         readHeader(bes, length, header.alternateTitleIds);
         break;
      case xex::Headers::AdditionalTitleMemory:
         readHeader(bes, length, header.additionalTitleMemory);
         break;
      case xex::Headers::ExportsByName:
         readHeader(bes, length, header.exportsByName);
         break;
      default:
         xDebug() << "Unknown header found 0x" << Log::hex(id, 8);
      }

      end = bes.tell();
      read = end - start;
      xDebug() << "Header 0x" << Log::hex(id, 8) << " read " << read << " / " << length << " bytes";

      bes.seek(pos);
   }

   bes.seek(header.securityInfoOffset);
   readHeader(bes, 0, header.loaderInfo);

   uint32_t sectionCount;
   bes >> sectionCount;

   for (uint32_t i = 0; i < sectionCount; ++i) {
      xex::Section section;

      bes 
         >> section._typePageCount
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

   Rijndael(xex2_retail_key).decrypt(header.loaderInfo.fileKey, header.sessionKey);

   /* Read Image */
   readImage(bes, header);

   /* Load windows PE from memory */
   loadPe(header.imageBaseAddress.address);
   loadImportLibraries(header.importLibraries);

   return true;
}

bool Loader::readImage(be::InputStream &bes, xex::Header &header)
{
   switch (header.baseFileFormat.compression.type) {
   case xex::BaseFileFormat::Compression::None:
      return readImageUncompressed(bes, header);
   case xex::BaseFileFormat::Compression::Basic:
      return readImageBasicCompression(bes, header);
   case xex::BaseFileFormat::Compression::Normal:
      return readImageNormalCompression(bes, header);
   default:
      xDebug() << "Unimplemented readImage compression type " << header.baseFileFormat.compression.type;
   }

   return false;
}

bool Loader::readImageUncompressed(be::InputStream &bes, xex::Header &header)
{
   return false;
}

bool Loader::readImageNormalCompression(be::InputStream &bes, xex::Header &header)
{
   return false;
}

bool Loader::readImageBasicCompression(be::InputStream &bes, xex::Header &header)
{
   uint32_t exeLength = bes.size();
   uint32_t uncompressedSize;

   bes.seek(header.peDataOffset);

   /* Calculate uncompressed size */
   uncompressedSize = 0;

   for (auto block : header.baseFileFormat.compression.basic.blocks) {
      uncompressedSize += block.dataSize + block.zeroSize;
   }

   /* Allocate virtual memory */
   uint8_t *pExecutable = be::Memory::allocate(header.imageBaseAddress.address, uncompressedSize);

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
            bes >> buffer;
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

bool Loader::loadPe(uint32_t exeAddress)
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
      section.name = stringFromFixedBuffer(secHeader->Name);
      section.rawAddress = secHeader->PointerToRawData;
      section.rawSize = secHeader->SizeOfRawData;
      section.address = exeAddress + secHeader->VirtualAddress;
      section.size = secHeader->Misc.VirtualSize;
      section.flags = secHeader->Characteristics;
      m_sections.push_back(section);
   }

   return true;
}

void Loader::loadImportLibraries(xex::ImportLibraries &importLibraries)
{
   for (auto &library : importLibraries.libraries) {
      for (auto &record : library.records) {
         const uint32_t value = be::Memory::read<uint32_t>(record.address);
         const uint32_t ordinal = value & 0xFFFF;
         const uint32_t type = value >> 24;

         switch (type) {
         case 0:
         {
            xex::ImportLibrary::Import import;
            import.ordinal = ordinal;
            library.imports.push_back(import);
            break;
         }
         case 1:
         {
            xex::ImportLibrary::Import &import = library.imports.back();
            import.thunk = record.address;

            /* Maybe not all ordered and have to search for ordinal ?? */
            assert(import.ordinal == ordinal);
            break;
         }
         default:
            xDebug() << "Unknown ImportLibraries record type " << type;
         }
      }
   }
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::ResourceInfo &resourceInfo)
{
   bes 
      >> resourceInfo.titleId
      >> resourceInfo.address
      >> resourceInfo.size;
}
   
void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::BaseFileFormat &baseFileFormat)
{
   bes
      >> baseFileFormat.encryption.type
      >> baseFileFormat.compression.type;

   switch (baseFileFormat.compression.type) {
   case xex::BaseFileFormat::Compression::Basic:
      {
         uint32_t blocks = (length - 4) / 8;

         for (uint32_t i = 0; i < blocks; ++i) {
            xex::BaseFileFormat::BasicCompression::Block block;

            bes
               >> block.dataSize
               >> block.zeroSize;

            baseFileFormat.compression.basic.blocks.push_back(block);
         }
         break;
      }
   case xex::BaseFileFormat::Compression::Normal:
      bes
         >> baseFileFormat.compression.normal.windowSize
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

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::EntryPoint &entryPoint)
{
   bes
      >> entryPoint.address;
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::ImageBaseAddress &imageBaseAddress)
{
   bes
      >> imageBaseAddress.address;
}

void Loader::readStringTable(be::InputStream &bes, uint32_t length, std::vector<std::string> &stringTable)
{
   std::string curString;

   for (uint32_t i = 0; i < length; ++i) {
      uint8_t c;
      bes >> c;

      if (c) {
         curString.push_back(c);
      } else if (curString.length() || (i + 1) == length) {
         stringTable.push_back(curString);
         curString.clear();
      }
   }
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::ImportLibraries &importLibraries)
{
   uint32_t stringTableSize, libraryCount;
   std::vector<std::string> stringTable;

   bes
      >> stringTableSize
      >> libraryCount;

   readStringTable(bes, stringTableSize, stringTable);

   for (uint32_t i = 0; i < libraryCount; ++i) {
      uint16_t nameIndex, importCount;
      xex::ImportLibrary library;

      bes
         >> library.unknown
         >> library.digest
         >> library.importId
         >> library.curVersion.value
         >> library.minVersion.value
         >> nameIndex
         >> importCount;

      library.name = stringTable.at(nameIndex);
         
      for (uint16_t j = 0; j < importCount; ++j) {
         xex::ImportLibrary::Record record;
         bes >> record.address;
         library.records.push_back(record);
      }

      importLibraries.libraries.push_back(library);
   }
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::ChecksumTimestamp &checksumTimestamp)
{
   bes
      >> checksumTimestamp.checksum
      >> checksumTimestamp.timestamp;
}
   
void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::OriginalPeName &originalPeName)
{
   bes
      >> NullPadString(originalPeName.name);
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::StaticLibraries &staticLibraries)
{
   uint32_t count = length / 16;

   for (uint32_t i = 0; i < count; ++i) {
      xex::StaticLibrary library;

      bes
         >> FixedWidthString(library.name, 8)
         >> library.version.major
         >> library.version.minor
         >> library.version.build
         >> library.version._qfeApproved;

      staticLibraries.libraries.push_back(library);
   }
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::TlsInfo &tlsInfo)
{
   bes
      >> tlsInfo.slotCount
      >> tlsInfo.rawDataAddress
      >> tlsInfo.dataSize
      >> tlsInfo.rawDataSize;
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::DefaultStackSize &defaultStackSize)
{
   bes
      >> defaultStackSize.size;
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::DefaultHeapSize &defaultHeapSize)
{
   bes
      >> defaultHeapSize.size;
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::SystemFlags &systemFlags)
{
   bes
      >> systemFlags.flags;
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::ExecutionInfo &executionInfo)
{
   bes
      >> executionInfo.mediaId
      >> executionInfo.curVersion.value
      >> executionInfo.baseVersion.value
      >> executionInfo.titleId
      >> executionInfo.platform
      >> executionInfo.executableTable
      >> executionInfo.discNumber
      >> executionInfo.discCount
      >> executionInfo.saveGameId;
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::GameRatings &gameRatings)
{
   bes
      >> gameRatings.esrb
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

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::LanKey &lanKey)
{
   bes
      >> lanKey.key;
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::AlternateTitleIds &alternateTitleIds)
{
   uint32_t count = length / 4;

   for (uint32_t i = 0; i < count; ++i) {
      uint32_t id;
      bes >> id;
      alternateTitleIds.ids.push_back(id);
   }
}

void Loader::readHeader(be::InputStream &bes, uint32_t length, xex::LoaderInfo &loaderInfo)
{
   bes 
      >> loaderInfo.headerSize
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
