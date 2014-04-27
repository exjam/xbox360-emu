#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <fstream>
#include <string>

#include "common/byte_swap.h"

namespace elf
{

struct Elf64_Ehdr
{
   unsigned char e_ident[16];
   uint16_t e_type;
   uint16_t e_machine;
   uint32_t e_version;
   uint64_t e_entry;
   uint64_t e_phoff;
   uint64_t e_shoff;
   uint32_t e_flags;
   uint16_t e_ehsize;
   uint16_t e_phentsize;
   uint16_t e_phnum;
   uint16_t e_shentsize;
   uint16_t e_shnum;
   uint16_t e_shstrndx;
};

struct Elf64_Shdr
{
   uint32_t sh_name;
   uint32_t sh_type;
   uint64_t sh_flags;
   uint64_t sh_addr;
   uint64_t sh_offset;
   uint64_t sh_size;
   uint32_t sh_link;
   uint32_t sh_info;
   uint64_t sh_addralign;
   uint64_t sh_entsize;
};

bool load(const std::string &path, uint64_t *sizeOut, uint8_t **codeOut)
{
   std::ifstream in;
   Elf64_Ehdr hdr;
   Elf64_Shdr txt;
   uint8_t *code;

   in.open(path, std::ifstream::in | std::ifstream::binary);

   if (!in.is_open()) {
      return false;
   }

   /* Read Big Endian Header */
   in.read(reinterpret_cast<char*>(&hdr), sizeof(Elf64_Ehdr));
   hdr.e_shoff = byte_swap(hdr.e_shoff);

   /* Read code section[1] */
   in.seekg(hdr.e_shoff + sizeof(Elf64_Shdr), in.beg);
   in.read(reinterpret_cast<char*>(&txt), sizeof(Elf64_Shdr));

   txt.sh_size = byte_swap(txt.sh_size);
   txt.sh_offset = byte_swap(txt.sh_offset);

   /* Read Code*/
   code = new uint8_t[txt.sh_size];
   in.seekg(txt.sh_offset, in.beg);
   in.read(reinterpret_cast<char*>(code), txt.sh_size);

   in.close();

   *sizeOut = txt.sh_size;
   *codeOut = code;
   return true;
}

} // namespace elf

#endif // ifndef ELF_H
