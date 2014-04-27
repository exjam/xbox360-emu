#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include "bit_field.h"
#include "endian.h"

template<typename Type, unsigned FieldWidth, unsigned Endian = LittleEndian>
class bit_array_t
{
public:
   bit_array_t()
   {
   }

   bit_array_t(Type value) :
      mValue(value)
   {
   }

   unsigned getFieldStart(unsigned index) const
   {
      return index * FieldWidth;
   }

   unsigned getFieldEnd(unsigned index) const
   {
      return (index * FieldWidth) + FieldWidth - 1;
   }

   unsigned getFieldWidth() const
   {
      return FieldWidth;
   }

   dyn_bit_field_t<Type> getField(unsigned index)
   {
      return bit_accessor_t<Endian>::make_bit_field(mValue,
                                                    getFieldStart(index),
                                                    getFieldEnd(index));
   }

   dyn_bit_field_t<Type> operator[](unsigned index)
   {
      return getField(index);
   }

private:
   Type mValue;
};

#endif // BIT_ARRAY_H
