#ifndef BIT_FIELD_H
#define BIT_FIELD_H

#include "bit_mask.h"

// Bit Field with compile time First and Last values
template<typename Type, unsigned First, unsigned Last>
class bit_field_t
{
   static const int Size = Last - First + 1;

public:
   bit_field_t(Type &src) :
      mBitField(src)
   {
   }

   unsigned getFirst() const
   {
      return First;
   }

   unsigned getSize() const
   {
      return Size;
   }

   unsigned getLast() const
   {
      return Last;
   }

   Type getValue() const
   {
      return (mBitField >> First)
         & bit_mask_t<Type, Size>::value;
   }

   void setValue(Type value)
   {
      const Type mask = bit_mask_t<Type, Size>::value;
      mBitField &= ~(mask << First);
      mBitField |= (value & mask) << First;
   }

   operator Type() const
   {
      return getValue();
   }

   bit_field_t<Type, First, Last> operator =(Type value)
   {
      setValue(value);
      return *this;
   }

private:
   Type &mBitField;
};

// Bit Field with run time First and Last values
template<typename Type>
class bit_field_t<Type, -1, -1>
{
public:
   bit_field_t(Type &src, unsigned first, unsigned last) :
      mBitField(src),
      mFirst(first),
      mLast(last)
   {
   }

   unsigned getFirst() const
   {
      return mFirst;
   }

   unsigned getSize() const
   {
      return mLast - mFirst + 1;
   }

   unsigned getLast() const
   {
      return mLast;
   }

   Type getValue() const
   {
      return (mBitField >> mFirst) & bit_mask<Type>(getSize());
   }

   void setValue(Type value)
   {
      const Type mask = bit_mask<Type>(getSize());
      mBitField &= ~(mask << mFirst);
      mBitField |= (value & mask) << mFirst;
   }

   operator Type() const
   {
      return getValue();
   }

   bit_field_t<Type, -1, -1> operator =(Type value)
   {
      setValue(value);
      return *this;
   }

private:
   Type &mBitField;
   unsigned mFirst;
   unsigned mLast;
};


template<typename Type>
using dyn_bit_field_t = bit_field_t<Type, -1, -1>;

#endif // BITFIELD_H
