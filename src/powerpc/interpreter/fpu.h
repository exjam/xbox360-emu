#ifndef PPC_FPU_H
#define PPC_FPU_H

#include "../../common/bits.h"

namespace ppc
{
   
namespace fpu
{

union Single
{
   Single(){}
   Single(float v) : v(v) {}
   Single(uint32_t uv) : uv(uv) {}

   float v;
   uint32_t uv;

   struct {
      uint32_t mantissa : 23;
      uint32_t exponent : 8;
      uint32_t sign : 1;
   };
};

union Double
{
   Double(){}
   Double(double v) : v(v) {}
   Double(uint64_t uv) : uv(uv) {}

   double v;
   uint64_t uv;

   struct {
      uint64_t mantissa : 52;
      uint64_t exponent : 11;
      uint64_t sign : 1;
   };
};

static const uint64_t exponent_min = 0;
static const uint64_t exponent_max = (1 << 11) - 1;

uint64_t exponent(double v)
{
   Double d = v;
   return d.exponent;
}

uint64_t mantissa(double v)
{
   Double d = v;
   return d.mantissa;
}

uint64_t sign(double v)
{
   Double d = v;
   return d.sign;
}

double makeNegative(double v)
{
   Double d = v;
   d.sign = 1;
   return d.v;
}

double makePositive(double v)
{
   Double d = v;
   d.sign = 0;
   return d.v;
}

double positiveZero()
{
   Double d;
   d.sign = 0;
   d.exponent = 0;
   d.mantissa = 0;
   return d.v;
}

double negativeZero()
{
   Double d;
   d.sign = 1;
   d.exponent = 0;
   d.mantissa = 0;
   return d.v;
}

double positiveInfinity()
{
   Double d;
   d.sign = 0;
   d.mantissa = 0;
   d.exponent = exponent_max;
   return d.v;
}

double negativeInfinity()
{
   Double d;
   d.sign = 1;
   d.mantissa = 0;
   d.exponent = exponent_max;
   return d.v;
}

double quietNaN()
{
   Double d;
   d.mantissa = 1ull << 51;
   d.exponent = exponent_max;
   return d.v;
}

double signalingNaN()
{
   Double d;
   d.mantissa = 1ull << 50;
   d.exponent = exponent_max;
   return d.v;
}

bool isPositive(double v)
{
   Double d = v;
   return d.sign == 0;
}

bool isNegative(double v)
{
   Double d = v;
   return d.sign == 1;
}

bool isZero(double v)
{
   Double d = v;
   return d.exponent == 0
       && d.mantissa == 0;
}

bool isPositiveZero(double v)
{
   Double d = v;
   return d.sign == 0
       && d.exponent == 0
       && d.mantissa == 0;
}

bool isNegativeZero(double v)
{
   Double d = v;
   return d.sign == 1
       && d.exponent == 0
       && d.mantissa == 0;
}

bool isNormal(double v)
{
   Double d = v;
   return d.exponent > exponent_min
       && d.exponent < exponent_max;
}

bool isDenormal(double v)
{
   Double d = v;
   return d.exponent == 0;
}

bool isInfinity(double v)
{
   Double d = v;
   return d.exponent == exponent_max
       && d.mantissa == 0;
}

bool isPositiveInfinity(double v)
{
   Double d = v;
   return d.sign == 0
       && d.exponent == exponent_max
       && d.mantissa == 0;
}

bool isNegativeInfinity(double v)
{
   Double d = v;
   return d.sign == 1
       && d.exponent == exponent_max
       && d.mantissa == 0;
}

bool isNaN(double v)
{
   Double d = v;
   return d.exponent == exponent_max
       && d.mantissa != 0;
}

bool isQuietNaN(double v)
{
   Double d = v;
   return d.exponent == exponent_max
       && d.mantissa != 0
       && (d.mantissa & (1ull << 51)) != 0;
}

bool isSignalingNaN(double v)
{
   Double d = v;
   return d.exponent == exponent_max
       && d.mantissa != 0
       && (d.mantissa & (1ull << 51)) == 0;
}

} // namespace fpu

} // namespace ppc

#endif // ifndef PPC_FPU_H
