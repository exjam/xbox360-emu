#ifndef COMMON_ptr32_t_H
#define COMMON_ptr32_t_H

#include <stdint.h>
#include <assert.h>
#include <type_traits>
#include "bit_cast.h"

#pragma pack(push, 1)

template<typename Type, typename StorageType = uint32_t>
class ptr32_t
{
public:
   ptr32_t()
   {
   }

   ptr32_t(Type *pointer)
   {
      setPointer(pointer);
   }

   ptr32_t<Type, StorageType> & operator =(Type *pointer)
   {
      setPointer(pointer);
      return *this;
   }

   template<typename OtherType>
   ptr32_t<Type, StorageType> & operator =(const ptr32_t<Type, OtherType> &pointer)
   {
      setPointer(pointer.getPointer());
      return *this;
   }

   void setPointer(Type *pointer)
   {
      uint64_t address = reinterpret_cast<uint64_t>(pointer);
      assert(address < 0xffffffff);

      #pragma warning(suppress : 4244)
      mPointer = static_cast<StorageType>(address);
   }

   Type *getPointer() const
   {
      uint64_t address = mPointer;
      return reinterpret_cast<Type *>(address);
   }

   StorageType getRawPointer() const
   {
      return mPointer;
   }

   Type &operator *() const
   {
      return *getPointer();
   }

   Type *operator ->() const
   {
      return getPointer();
   }

   operator Type *() const
   {
      return getPointer();
   }

   ptr32_t<Type, StorageType> & operator++()
   {
      mPointer += sizeof(Type);
      return *this;
   }

   ptr32_t<Type, StorageType> operator++(int)
   {
      ptr32_t<Type, StorageType> old { *this };
      mPointer += sizeof(Type);
      return old;
   }

private:
   StorageType mPointer;
};

// We shouldn't need to specialise here, but msvc seems to crash when
// we use std::enable_if<!std::is_void> on operator* and operator->
template<typename StorageType>
class ptr32_t<void, StorageType>
{
public:
   ptr32_t()
   {
   }

   ptr32_t(void *pointer)
   {
      setPointer(pointer);
   }

   void setPointer(void *pointer)
   {
      uint64_t address = reinterpret_cast<uint64_t>(pointer);
      assert(address < 0xffffffff);

      #pragma warning(suppress : 4244)
      mPointer = static_cast<StorageType>(address);
   }

   void *getPointer() const
   {
      uint64_t address = mPointer;
      return reinterpret_cast<void *>(address);
   }

   StorageType getRawPointer() const
   {
      return mPointer;
   }

   ptr32_t<void, StorageType> & operator =(void *pointer)
   {
      setPointer(pointer);
      return *this;
   }

   template<typename OtherType>
   ptr32_t<void, StorageType> & operator =(const ptr32_t<void, OtherType> &pointer)
   {
      setPointer(pointer.getPointer());
      return *this;
   }

   operator void *() const
   {
      return getPointer();
   }

private:
   StorageType mPointer;
};

// Check size of specialised ptr32_t<void> and generic ptr<T>
static_assert(sizeof(ptr32_t<void>) == 4, "ptr32_t<void> must be 4 bytes");
static_assert(sizeof(ptr32_t<uint64_t>) == 4, "ptr32_t<uint64_t> must be 4 bytes");

#pragma pack(pop)

#endif // COMMON_ptr32_t_H
