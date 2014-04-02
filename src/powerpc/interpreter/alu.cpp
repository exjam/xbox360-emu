#include "interpreter.h"
#include "regs.h"

#include "common/bits.h"

namespace ppc 
{

namespace Interpreter
{

template<typename Type>
static inline void updateCr0(State *state, Type value)
{
   auto flags = 0;

   if (value == 0) {
      flags |= ppc::Cr::Zero;
   } else if (bits::get(value, bits::count<Type>() - 1)) {
      flags |= ppc::Cr::Negative;
   } else {
      flags |= ppc::Cr::Positive;
   }

   if (state->reg.xer.so) {
      flags |= ppc::Cr::SummaryOverflow;
   }

   state->reg.cr.cr0 = flags;
}

static inline void updateXerOverflow(State *state, bool overflow)
{
   state->reg.xer.ov = overflow;
   state->reg.xer.so |= overflow;
}

/* Add x */
enum AddFlags 
{
   AddDefault    =      0,
   AddCarry      = 1 << 0,
   AddExtend     = 1 << 1,
   AddImmediate  = 1 << 2,
   AddShift      = 1 << 3,
   AddRecord     = 1 << 4,
   AddToZero     = 1 << 5,
   AddToMinusOne = 1 << 6,
};

template<int Flags>
bool addx(State *state, Instruction instr)
{
   uint64_t a, b, d;
   
   if (instr.rA == 0 && (Flags & AddImmediate) && (Flags & AddCarry) == 0) {
      a = 0;
   } else {
      a = gpr(instr.rA);
   }

   if (Flags & AddImmediate) {
      b = bits::signExtend<16, uint64_t>(instr.simm);
   } else if (Flags & AddToZero) {
      b = 0;
   } else if (Flags & AddToMinusOne) {
      b = -1;
   } else {
      b = gpr(instr.rB);
   }

   if (Flags & AddShift) {
      b <<= 16;
   }

   d = a + b;

   if (Flags & AddExtend) {
      d += state->reg.xer.ca;
   }

   gpr(instr.rD) = d;

   if ((Flags & AddImmediate) == 0) {
      if (instr.oe) {
         updateXerOverflow(state, bits::get((a ^ d) & (b ^ d), 63) != 0);
      }
   }

   if ((Flags & AddImmediate) == 0 || (Flags & AddRecord)) {
      if (instr.rc) {
         updateCr0(state, gpr(instr.rD));
      }
   }

   if (Flags & AddCarry || Flags & AddExtend) {
      state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA) ? 1 : 0;
   }

   return true;
}

/* Add */
bool add(State *state, Instruction instr)
{
   return addx<AddDefault>(state, instr);
}

/* Add Carrying */
bool addc(State *state, Instruction instr)
{
   return addx<AddCarry>(state, instr);
}

/* Add Extended */
bool adde(State *state, Instruction instr)
{
   return addx<AddExtend>(state, instr);
}

/* Add Immediate */
bool addi(State *state, Instruction instr)
{
   return addx<AddImmediate>(state, instr);
}

/* Add Immediate Carrying */
bool addic(State *state, Instruction instr)
{
   return addx<AddImmediate | AddCarry>(state, instr);
}

/* Add Immediate Carrying (record) */
bool addico(State *state, Instruction instr)
{
   return addx<AddImmediate | AddCarry | AddRecord>(state, instr);
}

/* Add Immediate Shifted */
bool addis(State *state, Instruction instr)
{
   return addx<AddImmediate | AddShift>(state, instr);
}

/* Add Minus One Extend */
bool addme(State *state, Instruction instr)
{
   return addx<AddToMinusOne | AddExtend>(state, instr);
}

/* Add Zero Extended */
bool addze(State *state, Instruction instr)
{
   return addx<AddToZero | AddExtend>(state, instr);
}

/* AND */
bool and(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) & gpr(instr.rB);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* AND with Complement */
bool andc(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) & ~gpr(instr.rB);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* And Immediate */
bool andio(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) & instr.uimm;
   updateCr0(state, gpr(instr.rA));
   return true;
}

/* And Immediate Shifted */
bool andiso(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) & (static_cast<uint64_t>(instr.uimm) << 16);
   updateCr0(state, gpr(instr.rA));
   return true;
}

/* Count Leading Zeroes Doubleword */
bool cntlzd(State *state, Instruction instr)
{
   auto a = gpr(instr.rS);
   gpr(instr.rA) = bits::beScanForward(a);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* Count Leading Zeroes Word */
bool cntlzw(State *state, Instruction instr)
{
   auto a = bits::zeroExtend<32>(gpr(instr.rS));
   gpr(instr.rA) = bits::beScanForward(a);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* Divide */
template<typename Type>
bool divxx(State *state, Instruction instr)
{
   Type a = reinterpret<Type>(gpr(instr.rA));
   Type b = reinterpret<Type>(gpr(instr.rB));

   bool overflow = b == 0;

   if (std::numeric_limits<Type>::is_signed) {
      if (b == -1 && a == (static_cast<Type>(1) << (bits::count<Type>() - 1))) {
         overflow = true;
      }
   }

   if (!overflow) {
      Type r = a / b;
      reinterpret<Type>(gpr(instr.rD)) = r;

      if (instr.rc) {
         updateCr0(state, r);
      }
   }

   if (instr.oe) {
      updateXerOverflow(state, overflow);
   }

   return true;
}

/* Divide Doubleword (signed) */
bool divd(State *state, Instruction instr)
{
   return divxx<int64_t>(state, instr);
}

/* Divide Doubleword Unsigned */
bool divdu(State *state, Instruction instr)
{
   return divxx<uint64_t>(state, instr);
}

/* Divide Word (signed) */
bool divw(State *state, Instruction instr)
{
   return divxx<int32_t>(state, instr);
}

/* Divide Word Unsigned */
bool divwu(State *state, Instruction instr)
{
   return divxx<uint32_t>(state, instr);
}

/* Equivalent */
bool eqv(State *state, Instruction instr)
{
   gpr(instr.rA) = ~(gpr(instr.rS) ^ gpr(instr.rB));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* Extend Sign */
template<int SrcBits>
bool extsx(State *state, Instruction instr)
{
   gpr(instr.rA) = bits::signExtend<SrcBits>(gpr(instr.rS));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* Extend Sign Byte */
bool extsb(State *state, Instruction instr)
{
   return extsx<8>(state, instr);
}

/* Extend Sign Halfword */
bool extsh(State *state, Instruction instr)
{
   return extsx<16>(state, instr);
}

/* Extend Sign Word */
bool extsw(State *state, Instruction instr)
{
   return extsx<32>(state, instr);
}

/* Multiply High Doubleword (signed) */
bool mulhd(State *state, Instruction instr)
{
   gprs(instr.rD) = __mulh(gprs(instr.rA), gprs(instr.rB));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   return true;
}

/* Multiply High Doubleword Unsigned */
bool mulhdu(State *state, Instruction instr)
{
   gpr(instr.rD) = __umulh(gpr(instr.rA), gpr(instr.rB));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   return true;
}

/* Multiply High Word (signed) */
bool mulhw(State *state, Instruction instr)
{
   auto a = gprsw(instr.rA);
   auto b = gprsw(instr.rB);
   auto r = static_cast<int64_t>(a * b) >> 32;

   gprsw(instr.rD) = static_cast<int32_t>(r);

   if (instr.rc) {
      updateCr0(state, gprw(instr.rD));
   }

   return true;
}

/* Multiply High Word Unsigned */
bool mulhwu(State *state, Instruction instr)
{
   auto a = gprw(instr.rA);
   auto b = gprw(instr.rB);
   auto r = static_cast<uint64_t>(a * b) >> 32;

   gprw(instr.rD) = static_cast<uint32_t>(r);

   if (instr.rc) {
      updateCr0(state, gprw(instr.rD));
   }

   return true;
}

/* Multiply Low Doubleword (signed) */
bool mulld(State *state, Instruction instr)
{
   auto a = gprs(instr.rA);
   auto b = gprs(instr.rB);
   auto r = a * b;

   gprs(instr.rD) = r;

   if (instr.rc) {
      updateCr0(state, gpr(instr.rD));
   }

   if (instr.oe) {
      updateXerOverflow(state, a != 0 && (r / a) != b);
   }

   return true;
}

/* Multiply Low Immediate (signed) */
bool mulli(State *state, Instruction instr)
{
   auto a = gprs(instr.rA);
   auto b = bits::signExtend<16, ppc::sreg_t>(instr.simm);

   gprs(instr.rD) = a * b;

   return true;
}

/* Multiply Low Word (signed) */
bool mullw(State *state, Instruction instr)
{
   auto a = gprsw(instr.rA);
   auto b = gprsw(instr.rB);
   auto r = a * b;

   gprsw(instr.rD) = r;

   if (instr.rc) {
      updateCr0(state, gprw(instr.rD));
   }

   if (instr.oe) {
      updateXerOverflow(state, a != 0 && (r / a) != b);
   }

   return true;
}

/* NOT And */
bool nand(State *state, Instruction instr)
{
   gpr(instr.rA) = ~(gpr(instr.rS) & gpr(instr.rB));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* Negate */
bool neg(State *state, Instruction instr)
{
   if (gpr(instr.rA) == std::numeric_limits<int64_t>::max()) {
      gpr(instr.rD) = std::numeric_limits<int64_t>::min();

      if (instr.oe) {
         updateXerOverflow(state, true);
      }
   } else {
      gpr(instr.rD) = ~(gpr(instr.rA) + 1);
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* NOR */
bool nor(State *state, Instruction instr)
{
   gpr(instr.rA) = ~(gpr(instr.rS) | gpr(instr.rB));

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* OR */
bool or(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) | gpr(instr.rB);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* OR with Complement */
bool orc(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) | ~gpr(instr.rB);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* OR Immediate */
bool ori(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) | instr.uimm;
   return true;
}

/* OR Immediate Shifted */
bool oris(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) | (static_cast<uint64_t>(instr.uimm) << 16);
   return true;
}

/* Rotate Left Doubleword x */
enum RldxxFlags
{
   RldDefault        =      0,
   RldClearLeft      = 1 << 0,
   RldClear          = 1 << 1,
   RldClearRight     = 1 << 2,
   RldImmediate      = 1 << 3,
   RldMaskInsert     = 1 << 4,
};

template<int Flags>
bool rldxx(State *state, Instruction instr)
{
   uint64_t r, m;
   int32_t n;

   if (Flags & RldImmediate) {
      n = instr.shd04 | (instr.shd5 << 4);
   } else {
      n = gprw(instr.rB) & 0x3f;
   }

   r = _rotl64(gpr(instr.rS), n);

   if (Flags & RldClearLeft) {
      m = bits::mask<uint64_t>(63 - instr.mbd, 63 - 63);
   } else if (Flags & RldClearRight) {
      m = bits::mask<uint64_t>(63 - 0, 63 - instr.med);
   } else if (Flags & RldClear) {
      m = bits::mask<uint64_t>(63 - instr.mbd, n);
   }

   if (Flags & RldMaskInsert) {
      gpr(instr.rA) = (r & m) | (gpr(instr.rA) & ~m);
   } else {
      gpr(instr.rA) = r & m;
   }

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* Rotate Left Doubleword then Clear Left */
bool rldcl(State *state, Instruction instr)
{
   return rldxx<RldClearLeft>(state, instr);
}

/* Rotate Left Doubleword then Clear Right */
bool rldcr(State *state, Instruction instr)
{
   return rldxx<RldClearRight>(state, instr);
}

/* Rotate Left Doubleword Immediate then Clear */
bool rldic(State *state, Instruction instr)
{
   return rldxx<RldImmediate | RldClear>(state, instr);
}

/* Rotate Left Doubleword Immediate then Clear Left */
bool rldicl(State *state, Instruction instr)
{
   return rldxx<RldImmediate | RldClearLeft>(state, instr);
}

/* Rotate Left Doubleword Immediate then Clear Right */
bool rldicr(State *state, Instruction instr)
{
   return rldxx<RldImmediate | RldClearRight>(state, instr);
}

/* Rotate Left Doubleword Immediate then Mask Insert */
bool rldimi(State *state, Instruction instr)
{
   return rldxx<RldImmediate | RldClear | RldMaskInsert>(state, instr);
}

/* Rotate Left Word x */
enum RlwxxFlags
{
   RlwDefault    =      0,
   RlwImmediate  = 1 << 0,
   RlwMaskInsert = 1 << 1,
};

template<int Flags>
bool rlwxx(State *state, Instruction instr)
{
   uint32_t n, r, m, a;

   if (Flags & RlwImmediate) {
      n = instr.sh;
   } else {
      n = gpr(instr.rB) & 0x1f;
   }
   
   r = _rotl(gprw(instr.rS), n);

   m = bits::mask<uint32_t>(31 - instr.mb, 31 - instr.me);

   if (Flags & RlwMaskInsert) {
      a = (r & m) | (r & ~m);
   } else {
      a = r & m;
   }

   gpr(instr.rA) = a;

   if (instr.rc) {
      updateCr0(state, a);
   }

   return true;
}

/* Rotate Left Word Immediate then Mask Insert */
bool rlwimi(State *state, Instruction instr)
{
   return rlwxx<RlwImmediate | RlwMaskInsert>(state, instr);
}

/* Rotate Left Word Immediate then AND with Mask */
bool rlwinm(State *state, Instruction instr)
{
   return rlwxx<RlwImmediate>(state, instr);
}

/* Rotate Left Word then AND with Mask */
bool rlwnm(State *state, Instruction instr)
{
   return rlwxx<RlwDefault>(state, instr);
}

/* Shift x */
enum
{
   ShiftDefault = 0,
   ShiftLeft = 1 << 0,
   ShiftRight = 1 << 1,
   ShiftImmediate = 1 << 2,
   ShiftAlgebraic = 1 << 3,
};

template<typename Type, int Flags>
bool sxx(State *state, Instruction instr)
{
   Type a, n;

   n = gpr(instr.rB) & (bits::count<Type>() - 1);

   if (gpr(instr.rB) & bits::count<Type>()) {
      a = 0;
   } else if (Flags & ShiftRight) {
      a = reinterpret<Type>(gpr(instr.rS)) >> n;
   } else if (Flags & ShiftLeft) {
      a = reinterpret<Type>(gpr(instr.rS)) << n;
   }

   gpr(instr.rA) = a;

   if (instr.rc) {
      updateCr0(state, a);
   }

   return true;
}

/* Shift Left Doubleword */
bool sld(State *state, Instruction instr)
{
   return sxx<uint64_t, ShiftLeft>(state, instr);
}

/* Shift Left Word */
bool slw(State *state, Instruction instr)
{
   return sxx<uint32_t, ShiftLeft>(state, instr);
}

/* Shift Right Doubleword */
bool srd(State *state, Instruction instr)
{
   return sxx<uint64_t, ShiftRight>(state, instr);
}

/* Shift Right Word */
bool srw(State *state, Instruction instr)
{
   return sxx<uint32_t, ShiftRight>(state, instr);
}

/* Shift Right Algebraic x */
enum SraxFlags
{
   SraxDefault   =      0,
   SraxImmediate = 1 << 0,
};

template<typename Type, int Flags>
bool sraxx(State *state, Instruction instr)
{
   uint32_t n;
   int msb = bits::count<Type>() - 1;
   
   if (Flags & SraxImmediate) {
      if (bits::count<Type>() == 64) {
         n = instr.shd04 | (instr.shd5 << 4);
      } else {
         n = instr.sh;
      }
   } else {
      n = gpr(instr.rB) & msb;
   }

   gprs(instr.rA) = reinterpret<Type>(gpr(instr.rS)) >> n;

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   state->reg.xer.ca = (bits::get(gpr(instr.rS), msb)
                        && bits::get(gpr(instr.rA), msb)) ? 1 : 0;

   return true;
}

/* Shift Right Algebraic Doubleword */
bool srad(State *state, Instruction instr)
{
   return sraxx<int64_t, SraxDefault>(state, instr);
}

/* Shift Right Algebraic Doubleword Immediate */
bool sradi(State *state, Instruction instr)
{
   return sraxx<int64_t, SraxImmediate>(state, instr);
}

/* Shift Right Algebraic Word */
bool sraw(State *state, Instruction instr)
{
   return sraxx<int32_t, SraxDefault>(state, instr);
}

/* Shift Right Algebraic Word Immediate */
bool srawi(State *state, Instruction instr)
{
   return sraxx<int64_t, SraxImmediate>(state, instr);
}

/* Subtract from x */
enum SubfFlags
{
   SubDefault      =      0,
   SubCarry        = 1 << 0,
   SubExtend       = 1 << 1,
   SubImmediate    = 1 << 2,
   SubFromZero     = 1 << 3,
   SubFromMinusOne = 1 << 4,
};

template<int Flags>
bool subfx(State *state, Instruction instr)
{
   uint64_t a, b, d;

   a = ~gpr(instr.rA);

   if (Flags & SubImmediate) {
      b = bits::signExtend<16, uint64_t>(instr.simm);
   } else if (Flags & SubFromZero) {
      b = 0;
   } else if (Flags & SubFromMinusOne) {
      b = -1;
   } else {
      b = gpr(instr.rB);
   }
   
   d = a + b;

   if (Flags & SubExtend) {
      d += state->reg.xer.ca;
   } else {
      d += 1;
   }

   gpr(instr.rD) = d;

   if ((Flags & SubImmediate) == 0) {
      if (instr.oe) {
         updateXerOverflow(state, bits::get((a ^ d) & (b ^ d), 63) != 0);
      }

      if (instr.rc) {
         updateCr0(state, gpr(instr.rD));
      }
   }
   
   if (Flags & SubCarry || Flags & SubExtend) {
      state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA) ? 1 : 0;
   }

   return true;
}

/* Subtract from */
bool subf(State *state, Instruction instr)
{
   return subfx<SubDefault>(state, instr);
}

/* Subtract from Carrying */
bool subfc(State *state, Instruction instr)
{
   return subfx<SubCarry>(state, instr);
}

/* Subtract from Extended */
bool subfe(State *state, Instruction instr)
{
   return subfx<SubExtend>(state, instr);
}

/* Subtract from Immediate Carrying */
bool subfic(State *state, Instruction instr)
{
   return subfx<SubImmediate | SubCarry>(state, instr);
}

/* Subtract from Minus One Extended */
bool subfme(State *state, Instruction instr)
{
   return subfx<SubFromMinusOne | SubExtend>(state, instr);
}

/* Subtract from Zero Extended */
bool subfze(State *state, Instruction instr)
{
   return subfx<SubFromZero | SubExtend>(state, instr);
}

/* XOR */
bool xor(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) ^ gpr(instr.rB);

   if (instr.rc) {
      updateCr0(state, gpr(instr.rA));
   }

   return true;
}

/* XOR Immediate */
bool xori(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) ^ instr.uimm;
   return true;
}

/* XOR Immediate Shifted */
bool xoris(State *state, Instruction instr)
{
   gpr(instr.rA) = gpr(instr.rS) ^ (static_cast<uint64_t>(instr.uimm) << 16);
   return true;
}

} // namespace Interpreter

} // namespace ppc
