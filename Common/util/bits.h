#ifndef UTIL_BITS_H
#define UTIL_BITS_H

#include <stdint.h>
#include <stdlib.h>
#include <intrin.h>

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

/* Little endian input, big endian result */
static inline unsigned long beScanBackward(uint32_t value)
{
   unsigned long result;

   if (!_BitScanForward(&result, value)) {
      return 32;
   } else {
      return 31 - result;
   }
}

/* Little endian input, big endian result */
static inline unsigned long beScanBackward(uint64_t value)
{
   unsigned long result;

   if (!_BitScanForward64(&result, value)) {
      return 64;
   } else {
      return 63 - result;
   }
}

/* Little endian input, big endian result */
static inline unsigned long beScanForward(uint32_t value)
{
   unsigned long result;

   if (!_BitScanReverse(&result, value)) {
      return 32;
   } else {
      return 31 - result;
   }
}

/* Little endian input, big endian result */
static inline unsigned long beScanForward(uint64_t value)
{
   unsigned long result;

   if (!_BitScanReverse64(&result, value)) {
      return 64;
   } else {
      return 63 - result;
   }
}

template<int width>
static inline uint64_t signExtend(uint64_t v)
{
   return (v & (1 << (width - 1))) ? (v | ~((1ull << width) - 1)) : v;
}

template<int width>
static inline uint64_t zeroExtend(uint64_t v)
{
   return v  & (1ull << width);
}

template<int width, typename T>
static inline T leBit(T value, uint32_t bit)
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
