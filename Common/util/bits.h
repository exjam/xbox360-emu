#ifndef UTIL_BITS_H
#define UTIL_BITS_H

#include <stdint.h>
#include <stdlib.h>
#include <intrin.h>

namespace bits
{

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

/* Swap between little endian and big endian */
template <typename Type>
static inline Type swap(Type value)
{
   union
   {
      Type value;
      unsigned char u8[sizeof(Type)];
   } source, dest;

   source.value = value;

   for (size_t k = 0; k < sizeof(Type); k++) {
      dest.u8[k] = source.u8[sizeof(Type) - k - 1];
   }

   return dest.value;
}

template<>
static inline unsigned char swap(unsigned char value)
{
   return value;
}

template<>
static inline unsigned short swap(unsigned short value)
{
   return _byteswap_ushort(value);
}

template<>
static inline unsigned int swap(unsigned int value)
{
   return _byteswap_ulong(value);
}

template<>
static inline unsigned long swap(unsigned long value)
{
   return _byteswap_ulong(value);
}

template<>
static inline unsigned long long swap(unsigned long long value)
{
   return _byteswap_uint64(value);
}

template<>
static inline float swap(float value)
{
   union
   {
      float fv;
      uint32_t iv;
   } u;

   u.fv = value;
   u.iv = swap(u.iv);

   return u.fv;
}

template<>
static inline double swap(double value)
{
   union
   {
      double fv;
      uint64_t iv;
   } u;

   u.fv = value;
   u.iv = swap(u.iv);

   return u.fv;
}

/* Generate a mask of bits size */
template<typename Type>
Type mask(int bits)
{
   if (bits == count<Type>()) {
      return -1;
   }

   return (static_cast<Type>(1) << bits) - 1;
}

/* Generate a mask between start and end */
template<typename Type>
Type mask(int start, int end)
{
   if (end < start) {
      return mask<Type>(end, start);
   } else {
      return mask<Type>((end - start) + 1) << start;
   }
}

/* Get a bit */
template<typename Type>
int get(Type value, uint32_t bit)
{
   const uint8_t *bytes = reinterpret_cast<uint8_t*>(&value);
   return (bytes[bit / 8] >> (bit % 8)) & 1;
}

/* Set a bit to 1 */
template<typename Type>
Type set(Type value, uint32_t bit)
{
   uint8_t *bytes = reinterpret_cast<uint8_t*>(&value);
   bytes[bit / 8] |= 1 << (bit % 8);
   return *reinterpret_cast<Type*>(bytes);
}

/* Set a bit to 0 */
template<typename Type>
Type clear(Type value, uint32_t bit)
{
   uint8_t *bytes = reinterpret_cast<uint8_t*>(&value);
   bytes[bit / 8] &= ~(1 << (bit % 8));
   return *reinterpret_cast<Type*>(bytes);
}

/* SignExtend from width to DstType */
template<int width, typename DstType = uint64_t>
static inline DstType signExtend(DstType v)
{
   return get(v, width - 1) == 0 ? v : (v | ~mask<DstType>(width));
}

/* ZeroExtend from width to DstType */
template<int width, typename DstType>
static inline DstType zeroExtend(DstType v)
{
   return v  & (static_cast<DstType>(1) << width);
}

/* Number of bits in a type */
template<typename Type>
int count()
{
   return sizeof(Type) * 8;
}

template<typename SrcType, int Start = 0, int Size = 0>
struct field
{
   field(SrcType &value) :
      value(value)
   {
   }

   operator SrcType() const
   {
      return (value >> Start) & bits::mask<SrcType>(Size);
   }

   field &operator=(const SrcType &v)
   {
      auto mask = bits::mask<SrcType>(Size);
      value &= ~(mask << Start);
      value |= (v & mask) << Start;
      return *this;
   }

   template<typename Type, int Start, int Size>
   field &operator=(const field<Type, Start, Size> &o)
   {
      (*this) = static_cast<SrcType>(static_cast<Type>(o));
      return *this;
   }

   SrcType& value;
};

template<typename SrcType>
struct field<SrcType, 0, 0>
{
   field(SrcType &value, int start, int size) :
      value(value), start(start), size(size)
   {
   }

   operator SrcType() const
   {
      return (value >> start) & bits::mask<SrcType>(size);
   }

   field &operator=(const SrcType& v)
   {
      auto mask = bits::mask<SrcType>(size);
      value &= ~(mask << start);
      value |= (v & mask) << start;
      return *this;
   }

   template<typename Type, int Start, int Size>
   field &operator=(const field<Type, Start, Size> &o)
   {
      (*this) = static_cast<SrcType>(static_cast<Type>(o));
      return *this;
   }

   int start;
   int size;
   SrcType& value;
};

template<typename SrcType, int Start_, int End_>
struct range : public field<SrcType, Start_ < End_ ? Start_ : End_, Start_ < End_ ? 1 + End_ - Start_ : 1 + Start_ - End_>
{
   static const int Start = Start_ < End_ ? Start_ : End_;
   static const int Size = Start_ < End_ ? 1 + End_ - Start_ : 1 + Start_ - End_;

   range(SrcType &value) :
      field(value)
   {
   }

   range &operator=(const SrcType& v)
   {
      field<SrcType, Start, Size> &self = *this;
      self = v;
      return *this;
   }
};

template<int Start, int End, typename SrcType>
range<SrcType, Start, End> makeRange(SrcType &src)
{
   return range<SrcType, Start, End>(src);
}

template<int Bit, typename Type>
void copy(Type &dst, Type &src)
{
   dst &= ~(1 << Bit);
   dst |= (src & 1) << Bit;
}

template<int Start, int End, typename Type>
void copy(Type &dst, Type &src)
{
   makeRange<Start, End, Type>(dst) = makeRange<Start, End, Type>(src);
}

template<int Bit, typename Type>
void clear(Type &dst)
{
   dst &= ~(1 << Bit);
}

template<int Start, int End, typename Type>
void clear(Type &dst)
{
   range<SrcType, Start, End>(dst) = 0;
}

} // namespace bits

#endif // UTIL_BITS_H
