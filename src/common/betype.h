#ifndef BETYPE_H
#define BETYPE_H

#include "bits.h"

#pragma pack(push, 1)

template<typename Type>
struct BE
{
   operator Type() const
   {
      return bits::swap(mValue);
   }

   BE<Type> &operator=(const Type &v)
   {
      mValue = bits::swap(v);
      return *this;
   }

   Type mValue;
};

#pragma pack(pop)

#endif
