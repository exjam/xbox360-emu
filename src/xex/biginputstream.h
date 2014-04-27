#ifndef BIG_ENDIAN_INPUT_STREAM_H
#define BIG_ENDIAN_INPUT_STREAM_H

#include <string>
#include <assert.h>
#include <stdint.h>

#include "common/byte_swap.h"

struct NullPadString
{
   NullPadString(std::string &str) : str(str)
   {
   }
   std::string &str;
};

struct FixedWidthString
{
   FixedWidthString(std::string &str, int width) : str(str), width(width)
   {
   }
   std::string &str;
   int width;
};

class BigInputStream
{
public:
   BigInputStream(std::istream& stream) :
      mStream(stream)
   {
   }

   template<typename Type>
   BigInputStream & operator>>(Type& val)
   {
      mStream.read(reinterpret_cast<char*>(&val), sizeof(Type));
      val = byte_swap(val);
      return *this;
   }

   template<>
   BigInputStream & operator>>(NullPadString& val)
   {
      unsigned __int8 c;
      int i = 0;

      for (int i = 0; i < 256; ++i) {
         (*this) >> c;

         if (c) {
            val.str.push_back(c);
         } else if (((i + 1) % 4) == 0) {
            break;
         }
      }

      return *this;
   }

   template<>
   BigInputStream & operator>>(FixedWidthString& val)
   {
      char buf[256];
      assert(val.width + 1 < sizeof(buf));
      mStream.read(buf, val.width);
      buf[val.width] = 0;
      val.str = buf;
      return *this;
   }

   template<int N>
   BigInputStream & operator>>(uint8_t(&val)[N])
   {
      mStream.read(reinterpret_cast<char*>(val), N);
      return *this;
   }

   void read(uint8_t *buf, unsigned int size)
   {
      mStream.read(reinterpret_cast<char*>(buf), size);
   }

   void seek(unsigned int pos)
   {
      mStream.seekg(pos, mStream.beg);
   }

   void skip(int pos)
   {
      mStream.seekg(pos, mStream.cur);
   }

   unsigned int tell()
   {
      return (unsigned int)mStream.tellg();
   }

   unsigned int size()
   {
      std::streampos start, pos, end;
      pos = mStream.tellg();

      mStream.seekg(0, mStream.end);
      end = mStream.tellg();

      mStream.seekg(0, mStream.beg);
      start = mStream.tellg();

      mStream.seekg(pos, mStream.beg);
      return (unsigned int)(end - start);
   }

private:
   std::istream& mStream;
};

#endif // ifdef BIG_ENDIAN_INPUT_STREAM_H
