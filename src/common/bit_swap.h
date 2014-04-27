#ifndef BIT_SWAP_H
#define BIT_SWAP_H

#include <stdint.h>

// Compile-time get bit position in opposite endian
// e.g. for uint32_t 31 <=> 0 and 15 <=> 16
template<typename Type, unsigned Bit>
struct bit_swap_t
{
   static const int value = sizeof(Type) * 8 - Bit - 1;
};

static_assert(bit_swap_t<uint32_t, 31>::value == 0,
              "bit_swapper<uint32_t, 31> != 0");

static_assert(bit_swap_t<uint32_t, 15>::value == 16,
              "bit_swapper<uint32_t, 15> != 16");

// Get bit position in opposite endian
template<typename Type>
unsigned bit_swap(unsigned bit)
{
   return sizeof(Type) * 8 - bit - 1;
}

#endif // BIT_SWAP_H
