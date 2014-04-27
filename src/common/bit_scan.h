#ifndef BIT_SCAN_H
#define BIT_SCAN_H

#include <stdint.h>
#include <intrin.h>

template<typename Type>
static inline unsigned bitScanReverse(Type value);

template<typename Type>
static inline unsigned bitScanForward(Type value);

template<>
static inline unsigned bitScanReverse<uint32_t>(uint32_t value)
{
   unsigned long index;

   if (!_BitScanReverse(&index, value)) {
      return 32;
   } else {
      return index;
   }
}

template<>
static inline unsigned bitScanReverse<uint64_t>(uint64_t value)
{
   unsigned long index;

   if (!_BitScanReverse64(&index, value)) {
      return 64;
   } else {
      return index;
   }
}

template<>
static inline unsigned bitScanForward<uint32_t>(uint32_t value)
{
   unsigned long index;

   if (!_BitScanForward(&index, value)) {
      return 32;
   } else {
      return index;
   }
}

template<>
static inline unsigned bitScanForward<uint64_t>(uint64_t value)
{
   unsigned long index;

   if (!_BitScanForward64(&index, value)) {
      return 64;
   } else {
      return index;
   }
}

#endif
