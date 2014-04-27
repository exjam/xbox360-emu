#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>

#include "ptr32.h"
#include "bit_field.h"
#include "bit_swap.h"
#include "bit_mask.h"
#include "bit_scan.h"

enum Endian
{
   LittleEndian,
   BigEndian
};

// Common bit functions specialised for LittleEndian or BigEndian
template<int Endian>
class bit_accessor_t;

template<>
class bit_accessor_t<LittleEndian>
{
public:
   template<typename Type, unsigned Size>
   static inline Type
   make_bit_mask()
   {
      if (width<Type>() == Size) {
         return -1;
      } else {
         return (static_cast<Type>(1) << Size) - 1;
      }
   }

   template<typename Type>
   static inline Type
   make_bit_mask(unsigned size)
   {
      assert(size <= width<Type>());

      if (width<Type>() == size) {
         return -1;
      } else {
         return (static_cast<Type>(1) << size) - 1;
      }
   }

   template<typename Type>
   static inline Type
   make_bit_mask(unsigned first, unsigned last)
   {
      assert(first <= last);
      return make_bit_mask<Type>((last - first) + 1) << first;
   }

   template<unsigned First, unsigned Last, typename Type>
   static inline auto
   make_bit_field(Type &src)
      -> bit_field_t<Type, First, Last>
   {
      static_assert(First <= Last, "make_bit_field<First, Last> ensure First <= Last");
      return bit_field_t<Type, First, Last> { src };
   }

   template<typename Type>
   static inline auto
   make_bit_field(Type &src, unsigned first, unsigned last)
      -> dyn_bit_field_t<Type>
   {
      assert(first <= last);
      return dyn_bit_field_t<Type> { src, first, last };
   }

   template<typename Type>
   static inline unsigned width()
   {
      return sizeof(Type) * 8;
   }

   template<unsigned Bit, typename Type>
   static inline Type get_bit(Type src)
   {
      return (src >> (Bit)) & 1;
   }

   template<typename Type>
   static inline Type get_bit(Type src, unsigned bit)
   {
      assert(bit < width<Type>());
      return (src >> bit) & 1;
   }

   template<unsigned Bit, typename Type>
   static inline Type set_bit(Type src)
   {
      return src | (1 << Bit);
   }

   template<typename Type>
   static inline Type set_bit(Type src, unsigned bit)
   {
      assert(bit < width<Type>());
      return src | (1 << bit);
   }

   template<unsigned Bit, typename Type>
   static inline Type clear_bit(Type src)
   {
      return src & ~(1 << Bit);
   }

   template<typename Type>
   static inline Type clear_bit(Type src, unsigned bit)
   {
      assert(bit < width<Type>());
      return src & ~(1 << bit);
   }

   template<unsigned Bit, typename Type>
   static inline void copy_bit(Type &src, Type &dst)
   {
      copy_bits<Bit, Bit>(src, dst);
   }

   template<unsigned First, unsigned Last, typename Type>
   static inline void copy_bits(Type &src, Type &dst)
   {
      make_bit_field<First, Last>(dst) = make_bit_field<First, Last>(src);
   }

   template<unsigned Size, typename Type = uint64_t>
   static inline Type signExtend(Type v)
   {
      return get_bit<Size - 1>(v) == 0 ? v : (v | ~bit_mask_t<Type, Size>::value);
   }

   template<unsigned Size, typename Type>
   static inline Type zeroExtend(Type v)
   {
      return v & ((static_cast<Type>(1) << Size) - 1);
   }

   template<typename Type>
   static inline unsigned countLeadingZeros(Type value)
   {
      return bitScanReverse(value);
   }
};

template<>
class bit_accessor_t<BigEndian>
{
public:
   template<typename Type>
   static inline Type
   make_bit_mask(unsigned first, unsigned last)
   {
      return little_endian::make_bit_mask<Type>(bit_swap<Type>(last),
                                                bit_swap<Type>(first));
   }
};

using be_bit_accessor = bit_accessor_t<BigEndian>;
using le_bit_accessor = bit_accessor_t<LittleEndian>;

using big_endian      = be_bit_accessor;
using little_endian   = le_bit_accessor;

#endif // ENDIAN_H
