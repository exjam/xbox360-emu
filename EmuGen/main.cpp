#include "emugen.h"
#include <iostream>

#include "../Common/util/bits.h"

template<typename SrcType, int Start, int Size>
struct BitWrapper
{
   BitWrapper(SrcType &value) : value(value)
   {
   }

   operator SrcType()
   {
      return (value >> Start) & bits::mask<SrcType>(Size);
   }

   BitWrapper &operator=(const SrcType &v)
   {
      auto mask = bits::mask<SrcType>(Size);
      value &= ~(mask << Start);
      value |= (v & mask) << Start;
      return *this;
   }

   SrcType& value;
};

int main(int argc, char **argv)
{
   int bitField = 0;

   auto bw = BitWrapper<int, 31, 1>(bitField);
   bw = 0x1;
   int val = bw;

   if (argc < 3) {
      std::cout << "Usage: " << argv[0] << " <definition file> <output directory>" << std::endl;
      return 0;
   }

   EmuGen app;
   app.run(argv[1], argv[2]);

   return 0;
}
