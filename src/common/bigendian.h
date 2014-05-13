#ifndef BIG_ENDIAN_H
#define BIG_ENDIAN_H

#include <stdint.h>

#include "ptr32.h"
#include "byte_swap.h"

// Type which is stored in big endian, but accessed in little endian
template<typename Type>
class be_t
{
public:
   be_t()
   {
   }

   be_t(Type value)
   {
      setValue(value);
   }

   Type getValue() const
   {
      return byte_swap(mValue);
   }

   void setValue(Type value)
   {
      mValue = byte_swap(value);
   }

   operator Type() const
   {
      return getValue();
   }

   be_t<Type> &operator =(Type value)
   {
      setValue(value);
      return *this;
   }

   be_t<Type> &operator ++()
   {
      setValue(getValue() + 1);
      return *this;
   }

private:
   Type mValue;
};

// Some common BigEndian types
using be_int8_t = be_t<int8_t>;
using be_int16_t = be_t<int16_t>;
using be_int32_t = be_t<int32_t>;
using be_int64_t = be_t<int64_t>;

using be_uint8_t = be_t<uint8_t>;
using be_uint16_t = be_t<uint16_t>;
using be_uint32_t = be_t<uint32_t>;
using be_uint64_t = be_t<uint64_t>;

// A BigEndian pointer to Type
template<typename Type>
using be_ptr32_t = ptr32_t<Type, be_t<uint32_t>>;

#endif // BIG_ENDIAN_H
