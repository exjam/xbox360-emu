#include "ppc/interpreter.h"
#include "ppc/instructions.h"

#include "util/log.h"
#include "util/bits.h"
#include "util/be/memory.h"

#include <limits>

namespace ppc 
{

namespace Interpreter
{

#define UNIMPLEMENTED(insName) \
   bool insName (State *state, Instruction) { \
      xDebug() << "Unimplemented interpreter instruction "#insName ; \
      return false; \
   }

#define gpr(instr) state->reg.gpr[instr]
#define gprs(instr) reinterpret<int64_t>(state->reg.gpr[instr])
#define gprw(instr) reinterpret<uint32_t>(state->reg.gpr[instr])
#define gprsw(instr) reinterpret<int32_t>(state->reg.gpr[instr])

template<typename DstType, typename SrcType>
static inline DstType& reinterpret(SrcType& value)
{
   return *reinterpret_cast<DstType*>(&value);
}

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
      state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA);
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
   gpr(instr.rA) = gpr(instr.rS) & (instr.uimm << 16);
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

   if (std::numeric_limits<Type>::is_signed && ) {
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
   auto r = (a * b) >> 32;

   gprsw(instr.rD) = r;

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
   auto r = (a * b) >> 32;

   gprw(instr.rD) = r;

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
bool mulld(State *state, Instruction instr)
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
bool nand(State *state, Instruction instr)
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
   gpr(instr.rA) = gpr(instr.rS) | (instr.uimm << 16);
   return true;
}

UNIMPLEMENTED(rldcl);    /* Rotate Left Doubleword then Clear Left */
UNIMPLEMENTED(rldcr);    /* Rotate Left Doubleword then Clear Right */
UNIMPLEMENTED(rldc);     /* Rotate Left Doubleword Immediate then Clear */
UNIMPLEMENTED(rldicl);   /* Rotate Left Doubleword Immediate then Clear Left */
UNIMPLEMENTED(rldicr);   /* Rotate Left Doubleword Immediate then Clear Right */
UNIMPLEMENTED(rldimi);   /* Rotate Left Doubleword Immediate then Mask Insert */
UNIMPLEMENTED(rlwimi);   /* Rotate Left Word Immediate then Mask Insert */
UNIMPLEMENTED(rlwinm);   /* Rotate Left Word Immediate then AND with Mask */
UNIMPLEMENTED(rlwnm);    /* Rotate Left Word then AND with Mask */

UNIMPLEMENTED(sld);      /* Shift Left Doubleword */
UNIMPLEMENTED(slw);      /* Shift Left Word */
UNIMPLEMENTED(srad);     /* Shift Right Algebraic Doubleword */
UNIMPLEMENTED(sradi);    /* Shift Right Algebraic Doubleword Immediate */
UNIMPLEMENTED(sraw);     /* Shift Right Algebraic Word */
UNIMPLEMENTED(srawi);    /* Shift Right Algebraic Word Immediate */
UNIMPLEMENTED(srd);      /* Shift Right Doubleword */
UNIMPLEMENTED(srw);      /* Shift Right Word */

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
      state->reg.xer.ca = gpr(instr.rD) < gpr(instr.rA);
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
   gpr(instr.rA) = gpr(instr.rS) ^ (instr.uimm << 16);
   return true;
}

} // namespace Interpreter

} // namespace ppc
