#ifndef BIT_MASK_H
#define BIT_MASK_H

// Compile-time bitmask
template<typename Type, unsigned Size, unsigned Offset = 0>
struct bit_mask_t
{
   // Computes correct value even if Size == sizeof(Type) * 8
   static const Type value = ((((static_cast<Type>(1) << (Size - 1)) - 1) << 1) | 1) << Offset;
};

// Run-time bit mask
template<typename Type>
Type bit_mask(unsigned size, unsigned offset = 0)
{
   return ((static_cast<Type>(1) << size) - 1) << offset;
}

#endif // BIT_MASK_H
