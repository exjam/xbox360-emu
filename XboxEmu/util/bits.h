#ifndef UTIL_BITS_H
#define UTIL_BITS_H

#include <stdint.h>
#include <stdlib.h>

namespace bits
{

static inline uint16_t swap(uint16_t value)
{
   return _byteswap_ushort(value);
}

static inline uint32_t swap(uint32_t value)
{
   return _byteswap_ulong(value);
}

static inline uint64_t swap(uint64_t value) 
{
   return _byteswap_uint64(value);
}

template<int width, typename T>
static inline T signExtend(T v)
{
   return (v & (1 << (width - 1))) ? (v | ~((1ull << width) - 1)) : v;
}

template<int width, typename T>
static inline T leBit(T value, int bit)
{
   return (value >> bit) & 1;
}

template<int width, typename T>
static inline T beBit(T value, uint32_t bit)
{
   return (value >> (width - 1 - bit)) & 1;
}

} // namespace bits

#endif // UTIL_BITS_H
