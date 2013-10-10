#ifndef BIG_ENDIAN_MEMORY_H
#define BIG_ENDIAN_MEMORY_H

#include <stdint.h>
#include <windows.h>

#include "util/bits.h"

namespace be
{

class Memory {
public:
   static inline uint8_t* allocate(uint64_t address, uint32_t size)
   {
      return reinterpret_cast<uint8_t*>(VirtualAlloc(reinterpret_cast<LPVOID>(address), size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
   }

   template<typename T>
   static inline T read(uint64_t address)
   {
      return *reinterpret_cast<T*>(address);
   }

   template<>
   static inline uint16_t read(uint64_t address)
   {
      return bits::swap(*reinterpret_cast<uint16_t*>(address));
   }

   template<>
   static inline uint32_t read(uint64_t address)
   {
      return bits::swap(*reinterpret_cast<uint32_t*>(address));
   }

   template<>
   static inline uint64_t read(uint64_t address)
   {
      return bits::swap(*reinterpret_cast<uint64_t*>(address));
   }

   static inline void set(uint64_t address, uint8_t value, uint64_t size)
   {
      memset(reinterpret_cast<void*>(address), value, size);
   }

   static inline void zero(uint64_t address, uint64_t size)
   {
      memset(reinterpret_cast<void*>(address), 0, size);
   }

   template<typename T>
   static inline void write(uint64_t address, const T &value)
   {
      *reinterpret_cast<T*>(address) = value;
   }

   template<>
   static inline void write(uint64_t address, const uint16_t &value)
   {
      *reinterpret_cast<uint16_t*>(address) = bits::swap(value);
   }

   template<>
   static inline void write(uint64_t address, const uint32_t &value)
   {
      *reinterpret_cast<uint32_t*>(address) = bits::swap(value);
   }

   template<>
   static inline void write(uint64_t address, const uint64_t &value)
   {
      *reinterpret_cast<uint64_t*>(address) = bits::swap(value);
   }

private:
};

} //namespace be

#endif //BIG_ENDIAN_MEMORY_H
